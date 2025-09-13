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

static const unsigned char image_info_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0xff,0x00,0xff,0x7f,0x3e,0xff,0x7f,0x3f,0xff,0xff,0x3f,0xff,0xff,0x1f,0xff,0xff,0x8f,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};

// --- 用户自定义的应用 ---
// 这是一个简单的“关于”页面应用
class AboutApp : public IApplication {
private:
    PixelUI& m_ui;
public:
    AboutApp(PixelUI& ui) : m_ui(ui){};
    void draw() override {
        U8G2& u8g2 = m_ui.getU8G2();
        
        u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
        u8g2.setFontDirection(0);
        u8g2.drawUTF8(0, 15, "Hello World!");
        u8g2.drawUTF8(0, 40, "你好世界");		// Chinese "Hello World" 
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
    }

    void onExit() {
        m_ui.markFading();
    }
};

// --- 自注册机制 ---
// 使用一个静态的 AppRegistrar 对象来在 main() 之前自动注册本应用
static AppRegistrar registrar_about_app({
    .title = "App Info",
    .bitmap = image_info_bits, // TODO: Add an icon bitmap here
    
    // 关键点：提供一个创建 AboutApp 实例的工厂函数
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<AboutApp>(ui); 
    },
    
    .type = MenuItemType::App,
    .order = 3
});