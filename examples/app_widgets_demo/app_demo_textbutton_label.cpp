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
static const unsigned char image_icon_text_button_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0x17,0xff,0xfb,0xb7,0x5b,0xf1,0xb7,0xb5,0xfb,0xb7,0xb9,0xfb,0xb7,0x53,0xf7,0xff,0xff,0xff,0xff,0xff,0xff,0x0f,0x00,0xf0,0xf7,0xff,0xef,0x77,0x67,0xee,0x37,0xeb,0xed,0x77,0x6f,0xee,0x77,0xf7,0xed,0x37,0x42,0xee,0xf7,0xff,0xef,0x0e,0x00,0x70,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

class App_Button_Demo : public IApplication {
private:
    PixelUI& m_ui;

    TextButton button_onload;
    TextButton button_top;
    TextButton button_bottom;
    TextButton button_left;
    TextButton button_right;
    Label label_demo;
    
    int32_t anim_title_bar = 0;
    int32_t anim_title_x = -50;
    int32_t anim_analog_clock_x = 103;

    enum class slide_direction {
        slide_top,
        slide_bottom,
        slide_left,
        slide_right
    } m_slide = slide_direction::slide_top;

public:
    App_Button_Demo(PixelUI& ui, void* parameter): m_ui(ui), 
    button_onload(ui, 13, 14, 44, 14, "onLoad"),
    button_top(ui, 85, 2, 41, 14, "Top"),
    button_bottom(ui, 85, 18, 41, 14, "Bottom"),
    button_left(ui, 85, 34, 40, 14, "Left"),
    button_right(ui, 85, 49, 40, 14, "Right"),
    label_demo(ui, 10, 53, "Hello 世界!", POS::TOP)
    {}

    void draw() override {
        U8G2& u8g2 = m_ui.getU8G2();

        u8g2.drawHLine(0, 19, anim_title_bar);

        u8g2.setFont(u8g2_font_profont10_tr);
        u8g2.drawStr(10, 12, "Call Label");
        
        u8g2.setFont(u8g2_font_5x7_tr);
        switch (m_slide) {
            case slide_direction::slide_top:
                u8g2.drawStr(1, 62, "Slide: Top");
                break;
            case slide_direction::slide_bottom:
                u8g2.drawStr(1, 62, "Slide: Bottom");
                break;
            case slide_direction::slide_left:
                u8g2.drawStr(1, 62, "Slide: Left");
                break;
            case slide_direction::slide_right:
                u8g2.drawStr(1, 62, "Slide: Right");
                break;
        }

        u8g2.drawLine(73, 32, 0, 32);
        u8g2.drawLine(74, 1, 74, 62);

        button_onload.draw();
        button_top.draw();
        button_bottom.draw();
        button_left.draw();
        button_right.draw();
        label_demo.draw();
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

        m_ui.setContinousDraw(true);
        m_ui.markDirty(); 

        button_onload.onLoad();

        button_top.onLoad();
        button_bottom.onLoad();
        button_left.onLoad();
        button_right.onLoad();

        button_onload.setCallback([this]() {
            label_demo.onLoad();
        });

        button_left.setCallback([this]() {
            label_demo.setLoadPos(POS::LEFT);
            m_slide = slide_direction::slide_left;
        });

        button_right.setCallback([this]() {
            label_demo.setLoadPos(POS::RIGHT);
            m_slide = slide_direction::slide_right;
        });

        button_top.setCallback([this]() {
            label_demo.setLoadPos(POS::TOP);
            m_slide = slide_direction::slide_top;
        });

        button_bottom.setCallback([this]() {
            label_demo.setLoadPos(POS::BOTTOM);
            m_slide = slide_direction::slide_bottom;
        });

        m_ui.addWidgetToFocusManager(&button_onload);
        m_ui.addWidgetToFocusManager(&button_top);
        m_ui.addWidgetToFocusManager(&button_bottom);
        m_ui.addWidgetToFocusManager(&button_left);
        m_ui.addWidgetToFocusManager(&button_right);
    }

    void onResume() override {
        m_ui.setContinousDraw(true);
    }

    void onExit() override {
        m_ui.setContinousDraw(false);
    }
};

AppItem button_demo_app = AppItem::make<App_Button_Demo>("Label / Button Demo", image_icon_text_button_bits);
