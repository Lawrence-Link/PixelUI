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


#include "app_skeleton.h"

Greenhouse_App::Greenhouse_App(PixelUI& ui, void* parameter) :
m_ui(ui),
anim_coroutine([this](CoroutineContext& ctx) {
    CORO_BEGIN(ctx);
    CORO_DELAY(ctx, m_ui, 160, 100);
    btn_light.onLoad();
    m_ui.animate(anim_x2_clip_light, 81, 950, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    m_ui.animate(anim_x_water_pump, 25, 350, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    brace_plant.onLoad();
    CORO_DELAY(ctx, m_ui, 200, 200);
    m_ui.animate(anim_x2_clip_soil, 57, 650, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    CORO_DELAY(ctx, m_ui, 200, 300);
    CORO_END(ctx);}),
btn_light(m_ui, 30, 2, 23, 7, image_light_auto_bits),
btn_pump(m_ui, 42, 17, 15, 11, image_pump_auto_bits),
brace_plant(m_ui, 3, 24, 29, 24),
plant(m_ui, 4, 1, 21, 23, image_plant_bits)
{
        brace_plant.addChild(plant);

        btn_light.setCallback([this]() {
            state_lamp = static_cast<ControlState> (!(bool)state_lamp);
            btn_light.setSource((state_lamp == ControlState::AUTO) ? image_light_auto_bits : image_light_on_bits );
            if (state_lamp == ControlState::ON) btn_light.setSize(11,7);
            else btn_light.setSize(23, 7);
        });

        btn_pump.setCallback([this]() {
            state_pump = static_cast<ControlState> (!(bool)state_pump);
            btn_pump.setSource((state_pump == ControlState::AUTO) ? image_pump_auto_bits : image_pump_on_bits);
        });
}

void Greenhouse_App::onEnter(ExitCallback exitCallback) {
    IApplication::onEnter(exitCallback);
    m_ui.animate(anim_w_brd, anim_h_brd, 60, 37, 450, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);

    m_ui.addWidgetToFocusManager(&btn_light);
    m_ui.addWidgetToFocusManager(&btn_pump);

    anim_coroutine.start();
    m_ui.addCoroutine(&anim_coroutine);
}

void Greenhouse_App::onExit() {
    m_ui.clearAllAnimations();
}

bool Greenhouse_App::handleInput(InputEvent event) {
    if (event == InputEvent::BACK) {
        requestExit(); // Request to close the application
    }  
    return true; // Standard app input handling is always true
}

void Greenhouse_App::draw() 
{
    U8G2& u8g2 = m_ui.getU8G2();
    
    u8g2.drawRFrame(58, 16, anim_w_brd, anim_h_brd, 3);
    u8g2.drawFrame(122, 1, 4, 62);  // Progress bar frame
    u8g2.drawFrame(123, 45, 2, 17); // Progress bar body

    u8g2.drawXBM(6, 2, 22, 15, image_light_bits); // Draw lamp

    u8g2.drawXBM(anim_x_water_pump, 17, 18, 25, image_water_pump_bits); // Draw water pump
    // u8g2.drawXBM(30, 2, 23, 7, image_light_auto_bits); // Draw Lighting state (AUTO)

    /* Draw wires */
    u8g2.setClipWindow(30, 6, anim_x2_clip_light, 14);
    u8g2.drawXBM(30, 6, 50, 8, image_wire_light_bits);
    u8g2.setMaxClipWindow();
    u8g2.setClipWindow(34, 44, anim_x2_clip_soil, 48);
    u8g2.drawXBM(34, 44, 23, 4, image_wire_soil_bits);
    u8g2.setMaxClipWindow();
        
    /* Labels */
    u8g2.drawXBM(62, 19, 5, 11, image_tmpt_bits); // temperature icon
    u8g2.drawXBM(1, 52, 5, 11, image_tmpt_bits);
    u8g2.drawXBM(90, 20, 7, 10, image_humi_bits); // humidity icon
    u8g2.drawXBM(40, 53, 7, 10, image_humi_bits);
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(63, 41, "P");
    u8g2.drawStr(63, 50, "N");
    u8g2.drawStr(91, 41, "K");
    u8g2.drawStr(89, 50, "mg/Kg");
        
   /* Data drawing */
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawUTF8(9, 62, "20°C");
    u8g2.drawStr(50, 62, "52 %");
    u8g2.drawStr(83, 11, "120Lux");

    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawUTF8(69, 28, "20°C");
    u8g2.drawStr(99, 28, "80%");
    u8g2.drawStr(70, 41, "35"); // Phosphate
    u8g2.drawStr(70, 50, "113"); // Potassium
    u8g2.drawStr(97, 41, "150"); // Nitrogen

    u8g2.drawHLine(58, 32, anim_width_divider_line);

    btn_light.draw();
    btn_pump.draw();
    brace_plant.draw();
}

AppItem greenhouse_app = AppItem::make<Greenhouse_App>("Greenhouse", image_icon_greenhouse_bits);
