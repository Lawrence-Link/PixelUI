/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/app/IApplication.h"
#include "PixelUI.h"
#include "core/app/app_system.h"
#include "core/coroutine/Coroutine.h"

#include "focus/focus.h"

#include "widgets/num_scroll/num_scroll.h"
#include "widgets/analog_clock/analog_clock.h"
#include "widgets/text_button/text_button.h"
#include "widgets/label/label.h"

/** @brief Bitmap data for application info icon (12x12). */
static const unsigned char image_Icon_Anim_Demo_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xbf,0x7f,0xff,0x7f,0xff,0x7f,0x00,0xfe,0xff,0x7f,0xff,0xff,0xbf,0xff,0xff,0xff,0xff,0x57,0x92,0xf0,0x57,0x92,0xf0,0x57,0x92,0xf0,0x57,0x92,0xf0,0x57,0x92,0xf0,0x57,0x92,0xf0,0xff,0xff,0xff,0xff,0xff,0xff,0xbf,0xbf,0xff,0x5f,0xf9,0xf5,0x1f,0x95,0xf1,0x5e,0xb5,0x75,0x5e,0x15,0x75,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

class App_Anim_Demo : public IApplication {
private:
    PixelUI& m_ui;

    enum class easing_mode {
        FALL_LINEAR,
        FALL_QUAD_IN,
        FALL_QUAD_OUT,
        FALL_QUAD_INOUT,
        FALL_CUBIC_IN,
        FALL_CUBIC_OUT,
        FALL_CUBIC_INOUT,
        FALL_BOUNCE
    } m_slide = easing_mode::FALL_LINEAR;

    bool once_flag = false;

    int32_t anim_block_x = 107;
    int32_t anim_block_y = 4;
    int32_t anim_block_w = 18;
    int32_t anim_block_h = 18;

    int32_t page_switch_duration = 1000;

    char print_buf[20] = {0};

    int32_t prev_timestamp;
    int32_t curr_timestamp;
public:
    App_Anim_Demo(PixelUI& ui, void* parameter): m_ui(ui)
    {
        setEnterTransitionEnabled(true);
    }

    void draw() override {

        Canvas& u8g2 = m_ui.getCanvas();

        if (!once_flag) {
            once_flag = true;
            prev_timestamp = curr_timestamp = m_ui.getCurrentTime();
        }

        curr_timestamp = m_ui.getCurrentTime();

        u8g2.setFont(u8g2_font_helvB08_tr);

        int slide_int = static_cast<int>(m_slide);
        easing_mode ease_real = static_cast<easing_mode>((slide_int == 0) ? 7 : slide_int - 1);

        const char* easing_name = "";
        
        switch (ease_real) {
            case easing_mode::FALL_LINEAR:
                easing_name = "LINEAR";
                break;
            case easing_mode::FALL_QUAD_IN:
                easing_name = "EASE IN QUAD";
                break;
            case easing_mode::FALL_QUAD_OUT:
                easing_name = "EASE OUT QUAD";
                break;
            case easing_mode::FALL_QUAD_INOUT:
                easing_name = "EASE INOUT QUAD";
                break;
            case easing_mode::FALL_CUBIC_IN:
                easing_name = "EASE IN CUBIC";
                break;
            case easing_mode::FALL_CUBIC_OUT:
                easing_name = "EASE OUT CUBIC";
                break;
            case easing_mode::FALL_CUBIC_INOUT:
                easing_name = "EASE INOUT CUBIC";
                break;
            case easing_mode::FALL_BOUNCE:
                easing_name = "BOUNCE";
                break;
        }

        u8g2.drawStr(4, 27, easing_name);

        if (curr_timestamp - prev_timestamp >= 1000) {

            // reset block attributes
            anim_block_x = 107;
            anim_block_y = 4;
            anim_block_w = 18;
            anim_block_h = 18;

            EasingType current_easing_type;

            switch(m_slide) {
                case easing_mode::FALL_LINEAR:
                    current_easing_type = EasingType::LINEAR;
                    break;
                case easing_mode::FALL_QUAD_IN:
                    current_easing_type = EasingType::EASE_IN_QUAD;
                    break;
                case easing_mode::FALL_QUAD_OUT:
                    current_easing_type = EasingType::EASE_OUT_QUAD;
                    break;
                case easing_mode::FALL_QUAD_INOUT:
                    current_easing_type = EasingType::EASE_IN_OUT_QUAD;
                    break;
                case easing_mode::FALL_CUBIC_IN:
                    current_easing_type = EasingType::EASE_IN_CUBIC;
                    break;
                case easing_mode::FALL_CUBIC_OUT:
                    current_easing_type = EasingType::EASE_OUT_CUBIC;
                    break;
                case easing_mode::FALL_CUBIC_INOUT:
                    current_easing_type = EasingType::EASE_IN_OUT_CUBIC;
                    break;
                case easing_mode::FALL_BOUNCE:
                    current_easing_type = EasingType::EASE_OUT_BOUNCE;
                    break;
            }
            
            // Start the animation with the determined type
            m_ui.animate(anim_block_y, 41, 500, current_easing_type, PROTECTION::PROTECTED);

            prev_timestamp = curr_timestamp;
            m_slide = static_cast<easing_mode>((static_cast<int>(m_slide) + 1) % 8);
        }

        u8g2.drawRBox(107, anim_block_y, 19, 18, 4);

        u8g2.setFont(u8g2_font_5x8_tr);
        u8g2.drawStr(4, 12, "EASING DEMO");

        u8g2.drawLine(3, 15, 72, 15);

        u8g2.setFont(u8g2_font_5x8_tr);
        // sprintf(print_buf, "X = %d", anim_block_x);
        // u8g2.drawStr(5, 36, print_buf);
        snprintf(print_buf, sizeof(print_buf), "Y = %d", anim_block_y);
        u8g2.drawStr(5, 61, print_buf);
        // sprintf(print_buf, "W = %d", anim_block_w);
        // u8g2.drawStr(5, 52, print_buf);
        // sprintf(print_buf, "Y = %d", anim_block_h);
        // u8g2.drawStr(5, 60, print_buf);

        u8g2.drawLine(0, 0, 128 * (curr_timestamp - prev_timestamp) / page_switch_duration, 0);
    }

    bool handleInput(InputEvent event) override {
        // No widget has taken over input, execute the original focus management logic
        if (event == InputEvent::BACK) {
            requestExit();
        } 
        return true;
    }
    
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        m_ui.markDirty(); 
    }

    void onResume() override {
    }

    void onExit() override {
        m_ui.clearAllAnimations();
    }
};

AppItem anim_demo_app = AppItem::make<App_Anim_Demo>("Anim Demo", image_Icon_Anim_Demo_bits);
