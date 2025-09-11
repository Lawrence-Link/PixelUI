/*
 * Copyright (C) 2025 Lawrence Li
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

static const unsigned char image_sans3_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0xff,0x00,0xff,0x7f,0x3e,0xff,0x7f,0x3f,0xff,0xff,0x3f,0xff,0xff,0x1f,0xff,0xff,0x0f,0xff,0xff,0x87,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xe3,0xff,0xff,0xe7,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};


// --- 用户自定义的应用 ---
// 这是一个简单的“关于”页面应用
class Dynamic_Info : public IApplication {
private:
    PixelUI& m_ui;
public:
    Dynamic_Info(PixelUI& ui) : m_ui(ui) {};
    ~Dynamic_Info() = default;
    float Y_Title = 0, Y_Version = 0, Y_description = 0;
    void draw() override {
        U8G2& display = m_ui.getU8G2();
        
        display.setFont(u8g2_font_ncenB10_tr);
        display.drawStr(40, Y_Title, "PixelUI");

        display.setFont(u8g2_font_tom_thumb_4x6_mf);
        display.drawStr(40, Y_Version, "Version 1.0");
        display.drawStr(30, Y_description, "Press BACK to exit");
    }

    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit(); // 请求退出
            return true;
        }
        return false;
    }
    
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        m_ui.animate(Y_Title,        20, 600, EasingType::EASE_OUT_BOUNCE);
        m_ui.animate(Y_Version,      35, 700, EasingType::EASE_OUT_BOUNCE);
        m_ui.animate(Y_description,  58, 800, EasingType::EASE_OUT_BOUNCE);
    }
};

// --- 自注册机制 ---
// 使用一个静态的 AppRegistrar 对象来在 main() 之前自动注册本应用
static AppRegistrar registrar_about_app({
    .title = "About",
    .bitmap = image_sans3_bits, // TODO: Add an icon bitmap here
    
    // 关键点：提供一个创建 AboutApp 实例的工厂函数
    .createApp = [](PixelUI& ui) -> std::shared_ptr<IApplication> { 
        return std::make_shared<Dynamic_Info>(ui); 
    },
    
    .type = MenuItemType::App,
    .order = 2
});