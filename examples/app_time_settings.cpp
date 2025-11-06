/*
 * Copyright (C) 2025 Lawrence Link
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/app/IApplication.h"
#include "core/app/app_system.h"
#include "core/coroutine/Coroutine.h"

#include "focus/focus.h"

#include "widgets/num_scroll/num_scroll.h"
#include "widgets/analog_clock/analog_clock.h"
#include "widgets/text_button/text_button.h"
#include "widgets/label/label.h"

static const unsigned char image_Icon_Clock_bits[] = {0xf0,0xff,0x0f,0xfc,0xe7,0x3f,0x7e,0xff,0x7e,0xee,0xff,0x77,0xdf,0xff,0xfb,0xff,0xf7,0xff,0xfb,0xf7,0xdf,0xff,0xf7,0xff,0xff,0x77,0xff,0xff,0xb7,0xff,0xff,0xd7,0xff,0xfd,0xe7,0xbf,0xfd,0xc7,0xbf,0xff,0x8f,0xff,0xff,0x1f,0xff,0xff,0x3f,0xfe,0xff,0x7f,0xfc,0xfb,0xff,0xdc,0xff,0xff,0xff,0xdf,0xff,0xfb,0xee,0xff,0x77,0x7e,0xff,0x7e,0xfc,0xe7,0x3f,0xf0,0xff,0x0f};

class TimeSetting : public IApplication {
private:
    PixelUI& m_ui;
    NumScroll num_h, num_m, num_s;
    FocusManager m_focusman;
    Clock clock;
    TextButton button_sync;
    Label title;
    Coroutine animationCoroutine_;

    int32_t anim_title_bar = 0;
    int32_t anim_title_x = -50;
    int32_t anim_analog_clock_x = 103;

public:
    TimeSetting(PixelUI& ui): m_ui(ui), 
    num_h(ui,  1, 25, 24, 16), 
    num_m(ui, 27, 25, 24, 16), 
    num_s(ui, 53, 25, 24, 16), 
    m_focusman(ui),
    clock(ui, 103, 32, 20),
    button_sync(ui, 1, 44, 76, 17),
    title(ui, 3, 14, "RTC时间"),
    animationCoroutine_([this](CoroutineContext& ctx){
        CORO_BEGIN(ctx);
        CORO_DELAY(ctx, m_ui, 100, 1); // wait for renderer loading
        clock.onLoad();
        num_h.onLoad();
        CORO_DELAY(ctx, m_ui, 100, 12);
        title.onLoad();
        num_m.onLoad();
        m_ui.animate(anim_title_bar, 78, 700, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
        m_ui.animate(anim_title_x, 3, 300, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
        // m_ui.animate(anim_analog_clock_x, 100, 300, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
        CORO_DELAY(ctx, m_ui, 100, 123);
        num_s.onLoad();
        CORO_DELAY(ctx, m_ui, 100, 11);
        button_sync.onLoad();
        CORO_END(ctx);
    }) {};

    void draw() override {
        U8G2& u8g2 = m_ui.getU8G2();

        // u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawHLine(0, 19, anim_title_bar);
        // u8g2.drawUTF8(anim_title_x, 16, "RTC时间");

        clock.setHour(num_h.getValue());
        clock.setMinute(num_m.getValue());
        clock.setSecond(num_s.getValue());

        num_h.draw();
        num_m.draw();
        num_s.draw();
        clock.draw();
        button_sync.draw();
        title.draw();
        m_focusman.draw();
    }

    bool handleInput(InputEvent event) override {
        IWidget* activeWidget = m_focusman.getActiveWidget();
        if (activeWidget) {
            // If so, pass the event to that widget
            if (activeWidget->handleEvent(event)) {
                // If the widget returns true, it means it has finished processing and control is handed back to the FocusManager
                m_focusman.clearActiveWidget();
            }
            return true; // Event has been handled, return true
        }
        // No widget has taken over input, execute the original focus management logic
        if (event == InputEvent::BACK) {
            requestExit();
        } else if (event == InputEvent::RIGHT) {
            m_focusman.moveNext();
        } else if (event == InputEvent::LEFT) {
            m_focusman.movePrev();
        } else if (event == InputEvent::SELECT) {
            m_focusman.selectCurrent();
        }
        return true;
    }
    
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        m_ui.setContinousDraw(true);
        m_ui.markDirty(); 

        num_h.setRange(0,23);
        num_h.setValue(0);
        num_h.setFixedIntDigits(2);

        num_m.setRange(0,59);
        num_m.setValue(0);
        num_m.setFixedIntDigits(2);

        num_s.setRange(0,59);
        num_s.setValue(0);
        num_s.setFixedIntDigits(2);

        clock.setHour(2);
        clock.setMinute(3);
        clock.setSecond(0);

        button_sync.setText("写入");

        animationCoroutine_.reset();
        animationCoroutine_.start();
        m_ui.addCoroutine(&animationCoroutine_);

        m_focusman.addWidget(&num_h);
        m_focusman.addWidget(&num_m);
        m_focusman.addWidget(&num_s);
        m_focusman.addWidget(&button_sync);
    }

    void onResume() override {
        m_ui.setContinousDraw(true);
    }

    void onExit() override {
        m_ui.setContinousDraw(false);

        // cleanup the coroutine
        m_ui.removeCoroutine(&animationCoroutine_);
    }
};
#if USE_STATIC_APP_REGISTER_ENABLED
static AppRegistrar time_setting_app({
    .title = "TimeSetting Demo",
    .bitmap = image_Icon_Clock_bits,
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> {
        return std::make_unique<TimeSetting>(ui);
    },
    .order = 5
});
#else
AppItem time_setting_app{
    .title = "TimeSetting Demo",
    .bitmap = image_Icon_Clock_bits,
    
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<TimeSetting>(ui); 
    },
};
#endif
