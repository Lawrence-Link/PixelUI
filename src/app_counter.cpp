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

#include "PixelUI/core/app/IApplication.h"
#include "PixelUI/core/app/app_system.h"
#include <memory>
#include <etl/stack.h>
#include "PixelUI/core/widgets/histogram/histogram.h"
#include "PixelUI/core/widgets/brace/brace.h"

static const unsigned char image_info_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0xff,0x00,0xff,0x7f,0x3e,0xff,0x7f,0x3f,0xff,0xff,0x3f,0xff,0xff,0x1f,0xff,0xff,0x8f,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};

static const unsigned char image_Background_bits[] = {0xfe,0x01,0x00,0x00,0x00,0x00,0x00,0xe0,0xff,0xff,0xff,0x0f,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x7d,0x06,0x00,0x00,0x00,0x00,0x00,0x18,0xff,0xb7,0x55,0x31,0x00,0x00,0x00,0x00,0x81,0xfc,0xff,0xff,0xff,0xff,0xff,0x8f,0x00,0x00,0x00,0xe2,0xff,0xff,0xff,0x7f,0x3d,0x01,0x00,0x00,0x00,0x00,0x00,0x40,0xb6,0xea,0xff,0x04,0x00,0x00,0x00,0x80,0x41,0xfe,0xff,0xff,0xaa,0xfe,0xff,0x3f,0x01,0x00,0x00,0xf9,0xff,0xff,0xff,0xab,0x9f,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xf8,0xff,0x7f,0x02,0x00,0x00,0x00,0x80,0x20,0xff,0xff,0xff,0xff,0x55,0xfd,0x7f,0xfc,0xff,0xff,0x6c,0xff,0xff,0xff,0xb5,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x80,0x01,0x00,0x00,0x00,0x80,0x80,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03,0x00,0x00,0xff,0xff,0xff,0xff,0xff};

static float s_static_data_buffer[25] = {
    0.1f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 0.9f, 0.7f, 0.5f, 0.3f,
    0.2f, 0.1f, 0.3f, 0.5f, 0.7f, 0.9f, 1.0f, 0.8f, 0.6f, 0.4f,
    0.2f, 0.1f, 0.2f, 0.3f, 0.4f
};

class APP_COUNTER: public IApplication {
private:
    PixelUI& m_ui;
    Histogram histogram;
    Brace brace;

    // State machine for loading animation sequence
    enum class LoadState {
        INIT,           // start
        BRACE_LOADING,  // execute brace.onLoad()
        WAIT_HISTO,     // wait 300ms
        HISTO_LOADING,  // execute histogram.onLoad()
        DONE            // all done
    } loadState = LoadState::INIT;

    uint32_t state_timestamp = 0;  // record time when entering a state
    bool first_time = false;

    // animation related variables
    int32_t anim_mark_m = 0;
    int32_t anim_bg = 0;
    int32_t anim_status_x = -27;
public:
    APP_COUNTER(PixelUI& ui) : m_ui(ui), histogram(ui), brace(ui) {}

    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);

        // HISTOGRAM
        histogram.setCoordinate(97,54);
        histogram.setMargin(56,18);
        
        // BRACE 
        brace.setCoordinate(31,54);
        brace.setMargin(56,18);
        brace.setDrawContentFunction([this]() { braceContent(); });

        loadState = LoadState::INIT;
        first_time = false;
    }

    void braceContent() {
        U8G2& u8g2 = m_ui.getU8G2();
        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(30, 54, "10.00");
        u8g2.drawStr(31, 61, "uSv/h");
        u8g2.drawRBox(8, 50, 20, 10, 2);
        u8g2.setDrawColor(0);
        u8g2.drawStr(11, 58, "Max");
        u8g2.setDrawColor(1);
    }

    void draw() override {
        if (!first_time) {
            // 启动进度动画
            m_ui.animate(anim_mark_m, 23, 300, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
            m_ui.animate(anim_bg, 128, 500, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
            
            // the state machine start from its initial state
            loadState = LoadState::BRACE_LOADING;
            state_timestamp = m_ui.getCurrentTime();
            first_time = true;
        }

        // state machine
        switch (loadState) {
            case LoadState::BRACE_LOADING:
                brace.onLoad();
                loadState = LoadState::WAIT_HISTO;
                state_timestamp = m_ui.getCurrentTime(); // record when was the state entered
                break;

            case LoadState::WAIT_HISTO:
                if (m_ui.getCurrentTime() - state_timestamp >= 80) {
                    loadState = LoadState::HISTO_LOADING;
                }
                break;

            case LoadState::HISTO_LOADING:
                histogram.onLoad();
                loadState = LoadState::DONE;
                m_ui.animate(anim_status_x, 29, 450, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
                break;

            case LoadState::DONE:
                // Do nothing, wait for exit
                break;

            default:
                break;
        }

        // UI drawing
        U8G2& u8g2 = m_ui.getU8G2();
        u8g2.setClipWindow(0,7,anim_bg,18);
        u8g2.drawXBM(0, 7, 128, 10, image_Background_bits);
        u8g2.setMaxClipWindow();
        
        histogram.setData(s_static_data_buffer, 25, 0);
        histogram.draw();
        brace.draw();

        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(5, 42, "MEAS");

        u8g2.setClipWindow(29,36,83,42);
        u8g2.drawStr(anim_status_x, 42, "PLEASE WAIT");
        u8g2.setMaxClipWindow();

        u8g2.setDrawColor(2);
        u8g2.drawBox(3, 35, anim_mark_m, 8);
        u8g2.setDrawColor(1);
        u8g2.setFont(u8g2_font_profont17_tr);
        u8g2.drawStr(3, 31, "0.25uSv/h");

        u8g2.setFont(u8g2_font_4x6_tr);
        u8g2.drawStr(100, 32, "CNT");
        u8g2.drawStr(100, 39, "1234");
    }

    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit();
            return true;
        }
        return false;
    }

    void onExit() {
        m_ui.markFading();
    }
};


static AppRegistrar registrar_about_app({
    .title = "COUNTER",
    .bitmap = image_info_bits,
    
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<APP_COUNTER>(ui); 
    },
    
    .type = MenuItemType::App,
    .order = 0
});