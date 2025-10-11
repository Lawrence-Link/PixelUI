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
#include <cmath> // Include cmath for potential future use

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
    Dynamic_Info(PixelUI& ui) : m_ui(ui) {};
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

// ---------------- Application registration ----------------
#if USE_STATIC_APP_REGISTER_ENABLED
static AppRegistrar registrar_about_app({
    .title = "About",
    .bitmap = image_info_bits, // TODO: Add an icon bitmap here

    // Provide a factory function to create application instance.
    .createApp = [](PixelUI& ui) -> std::shared_ptr<IApplication> {
        return std::make_shared<Dynamic_Info>(ui);
    },

    .order = 2
});
#else

AppItem bouncy_about_app{
    .title = "About",
    .bitmap = image_info_bits, // TODO: Add an icon bitmap here

    // Provide a factory function to create application instance.
    .createApp = [](PixelUI& ui) -> std::shared_ptr<IApplication> {
        return std::make_shared<Dynamic_Info>(ui);
    },
};

#endif
