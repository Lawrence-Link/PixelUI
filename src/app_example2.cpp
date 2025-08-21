#include "PixelUI/core/app/IApplication.h"  // App接口
#include "PixelUI/core/app/app_system.h"    // App系统
#include <memory>
#include <iostream>

static const unsigned char image_sans2_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0x7f,0x00,0xff,0x9f,0xff,0xfc,0xef,0xff,0xfb,0xef,0xff,0xfb,0xf7,0xff,0xf7,0xf7,0x3f,0xf6,0xf7,0xbf,0xf6,0x37,0x36,0xf6,0xef,0xe3,0xfb,0xa7,0xff,0xf2,0x37,0x00,0xf6,0x77,0x55,0xf7,0xcf,0x80,0xf9,0x1f,0x7f,0xfc,0x7f,0x00,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};


// --- 用户自定义的应用 ---
// 这是一个简单的“关于”页面应用
class AboutApp2 : public IApplication {
public:
    void draw() override {
        extern PixelUI ui;
        U8G2Wrapper& display = ui.getU8G2();
        
        display.setFont(u8g2_font_ncenB10_tr);
        display.drawStr(20, 20, "Pixel UI");

        display.setFont(u8g2_font_tom_thumb_4x6_mf);
        display.drawStr(30, 35, "Version 1.0");
        display.drawStr(15, 58, "Press BACK to exit");
    }

    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit();
            return true;
        }
        return false;
    }
    
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        std::cout << "[AboutApp] Entered." << std::endl;
    }
};

static AppRegistrar registrar_about_app({
    .title = "Sans2",
    .bitmap = image_sans2_bits, // TODO: Add an icon bitmap here
    
    // 关键点：提供一个创建 AboutApp 实例的工厂函数
    .createApp = []() -> std::shared_ptr<IApplication> { 
        return std::make_shared<AboutApp2>(); 
    },
    
    .type = MenuItemType::App,
    .w = 24, 
    .h = 24,
    .useUnifiedEnterAnimation = true
});