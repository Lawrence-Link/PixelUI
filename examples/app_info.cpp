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
#include <memory>

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
    AboutApp(PixelUI& ui) : m_ui(ui){};
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
    void onExit() {
        // No cleanup needed.
    }
};

// ---------------- Application registration ----------------
#if USE_STATIC_APP_REGISTER_ENABLED
static AppRegistrar registrar_about_app({
    .title = "App Info",
    .bitmap = image_info_bits,

    // Provide a factory function to create application instance.
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> {
        return std::make_unique<AboutApp>(ui);
    },

    .order = 3
});
#else

AppItem about_app{
    .title = "App Info",
    .bitmap = image_info_bits,

    // Provide a factory function to create application instance.
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> {
        return std::make_unique<AboutApp>(ui);
    },
};

#endif
