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

class TimeSetting : public IApplication {
private:
    PixelUI& m_ui;
    NumScroll num_h, num_m, num_s;
    FocusManager m_focusman;
    Clock clock;

    std::shared_ptr<Coroutine> animationCoroutine_;

    int32_t anim_title_bar = 0;
    int32_t anim_title_x = -50;
    int32_t anim_analog_clock_x = 180;
    void animation_load_coroutine(CoroutineContext& ctx, PixelUI& ui) {
        CORO_BEGIN(ctx);
        CORO_DELAY(ctx,ui, 100, 1); // wait for renderer loading
        clock.onLoad();
        num_h.onLoad();
        CORO_DELAY(ctx, ui, 100, 12);
        num_m.onLoad();
        m_ui.animate(anim_title_bar, 75, 700, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
        m_ui.animate(anim_title_x, 3, 300, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
        m_ui.animate(anim_analog_clock_x, 3, 300, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
        CORO_DELAY(ctx, ui, 100, 123);
        num_s.onLoad();
        CORO_END(ctx);
    }

public:
    TimeSetting(PixelUI& ui): m_ui(ui), 
    num_h(ui), 
    num_m(ui), 
    num_s(ui), 
    m_focusman(ui),
    clock(ui) {};
    void draw() override {
        U8G2& u8g2 = m_ui.getU8G2();

        u8g2.drawHLine(0, 16, anim_title_bar);
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawUTF8(anim_title_x, 12, "RTC时间");

        clock.setHour(num_h.getValue());
        clock.setMinute(num_m.getValue());
        clock.setSecond(num_s.getValue());

        num_h.draw();
        num_m.draw();
        num_s.draw();
        clock.draw();

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

        num_h.setPosition(3,45);
        num_h.setRange(0,23);
        num_h.setSize(24, 16);
        num_h.setValue(0);
        num_h.setFixedIntDigits(2);

        num_m.setPosition(32,45);
        num_m.setRange(0,59);
        num_m.setSize(24, 16);
        num_m.setValue(0);
        num_m.setFixedIntDigits(2);

        num_s.setPosition(61,45);
        num_s.setRange(0,59);
        num_s.setSize(24, 16);
        num_s.setValue(0);
        num_s.setFixedIntDigits(2);

        clock.setPosition(100,24);
        clock.setRadius(20);
        clock.setHour(2);
        clock.setMinute(3);
        clock.setSecond(0);

        animationCoroutine_ = std::make_shared<Coroutine>(
            std::bind(&TimeSetting::animation_load_coroutine, this, std::placeholders::_1, std::placeholders::_2), m_ui
        );

        m_ui.addCoroutine(animationCoroutine_);

        m_focusman.addWidget(&num_h);
        m_focusman.addWidget(&num_m);
        m_focusman.addWidget(&num_s);
    }

    void onResume() override {
        m_ui.setContinousDraw(true);
    }

    void onExit() override {
        m_ui.setContinousDraw(false);
        m_ui.markFading();

        // cleanup the coroutine
        if (animationCoroutine_) {
            m_ui.removeCoroutine(animationCoroutine_);
            animationCoroutine_.reset();
        }
    }
};
#if USE_STATIC_APP_REGISTER_ENABLED
static AppRegistrar time_setting_app({
    .title = "TimeSetting Demo",
    .bitmap = nullptr,
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> {
        return std::make_unique<TimeSetting>(ui);
    },
    .order = 5
});
#else
AppItem time_setting_app{
    .title = "TimeSetting Demo",
    .bitmap = nullptr,
    
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<TimeSetting>(ui); 
    },
};
#endif
