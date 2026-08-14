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
#include <etl/memory.h>

static const unsigned char image_info_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0xff,0x00,0xff,0x7f,0x3e,0xff,0x7f,0x3f,0xff,0xff,0x3f,0xff,0xff,0x1f,0xff,0xff,0x8f,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};

/**
 * @brief Simple "About" application demonstrating basic text drawing, including UTF-8 Chinese characters.
 */
class AboutApp : public IApplication {
private:
    /** @brief Reference to the main UI context. */
    PixelUI& m_ui;
public:
    /**
     * @brief Constructor for AboutApp.
     * @param ui Reference to the PixelUI instance.
     */
    AboutApp(PixelUI& ui, void* parameter) : m_ui(ui){};
    ~AboutApp() = default;

    /**
     * @brief Main drawing function, called to render content.
     * Displays "Hello World!" in English and Chinese.
     */
    void draw() override {
        U8G2& u8g2 = m_ui.getU8G2();

        // Use a font that supports the required Chinese characters.
        u8g2.setFont(u8g2_font_unifont_t_chinese2);
        u8g2.setFontDirection(0);

        // Draw English "Hello World!"
        u8g2.drawUTF8(0, 15, "Hello World!");

        // Draw Chinese "Hello World" (你好世界)
        u8g2.drawUTF8(0, 40, "你好世界");
    }

    /**
     * @brief Handles input events.
     * @param event The input event received.
     * @return true if the event was consumed, false otherwise.
     */
    bool handleInput(InputEvent event) override {
        // Check for the BACK button press to exit the application
        if (event == InputEvent::BACK) {
            requestExit(); // Request exit
            return true;
        }
        return false;
    }

    /**
     * @brief Setup function called when the application is entered.
     * @param cb Callback function to be executed upon application exit.
     */
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        // This app does not use continuous draw or animations.
    }

    /**
     * @brief Cleanup function called when the application exits.
     */
    void onExit() override {
        // No cleanup needed.
    }
};

AppItem about_app = AppItem::make<AboutApp>("App Info", image_info_bits);
