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
#include "core/app/app_system.h"
#include <etl/memory.h>
#include <math.h> // Include cmath for potential future use

static const unsigned char image_info_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0xff,0x00,0xff,0x7f,0x3e,0xff,0x7f,0x3f,0xff,0xff,0x3f,0xff,0xff,0x1f,0xff,0xff,0x0f,0xff,0xff,0x87,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xe3,0xff,0xff,0xe7,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

/**
 * @brief Application demonstrating a bouncing 'About' screen using animation.
 */
class Dynamic_Info : public IApplication {
private:
    /** @brief Reference to the main UI context. */
    PixelUI& m_ui;
public:
    /**
     * @brief Constructor for Dynamic_Info.
     * @param ui Reference to the PixelUI instance.
     */
    Dynamic_Info(PixelUI& ui, void* parameter) : m_ui(ui) {};
    ~Dynamic_Info() = default;

    /** @brief Y-coordinate for the main title text. */
    int32_t m_titleY = 0;
    /** @brief Y-coordinate for the version text. */
    int32_t m_versionY = 0;
    /** @brief Y-coordinate for the exit instruction text. */
    int32_t m_descriptionY = 0;

    /**
     * @brief Main drawing function, called to render content.
     */
    void draw() override {
        U8G2& display = m_ui.getU8G2();

        display.setFont(u8g2_font_ncenB10_tr);
        display.drawStr(40, m_titleY, "PixelUI");

        display.setFont(u8g2_font_tom_thumb_4x6_mf);
        display.drawStr(40, m_versionY, "Version 1.0");
        display.drawStr(30, m_descriptionY, "Press BACK to exit");
    }

    /**
     * @brief Handles input events.
     * @param event The input event received.
     * @return true if the event was consumed, false otherwise.
     */
    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit(); // 请求退出
            return true;
        }
        return false;
    }

    /**
     * @brief Setup function called when the application is entered.
     * Starts the bouncing animations for the text elements.
     * @param cb Callback function to be executed upon application exit.
     */
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        // Start animations using the optimized member variable names
        m_ui.animate(m_titleY,        20, 600, EasingType::EASE_OUT_BOUNCE);
        m_ui.animate(m_versionY,      35, 700, EasingType::EASE_OUT_BOUNCE);
        m_ui.animate(m_descriptionY,  58, 800, EasingType::EASE_OUT_BOUNCE);
    }
};

AppItem bouncy_about_app{
    .title = "About",
    .bitmap = image_info_bits, // TODO: Add an icon bitmap here

    // Provide a factory function to create application instance.
    .createApp = [](PixelUI& ui, void* parameter) -> etl::unique_ptr<IApplication> {
        return etl::unique_ptr<IApplication>(new Dynamic_Info(ui, parameter));
    },
};
