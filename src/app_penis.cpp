// app_example.cpp (或者任何用户自定义的 app_*.cpp 文件)
#include "PixelUI/core/app/IApplication.h"
#include "PixelUI/core/app/app_system.h"
#include <memory>
#include <iostream>

static const unsigned char image_sans4_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xfb,0x3f,0xfe,0xf7,0x7f,0xfe,0xeb,0x7f,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xe3,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0xff,0xdd,0xff,0x3f,0x1c,0xfe,0xbf,0xdd,0xfe,0xbf,0xdd,0xfe,0x7e,0x3e,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

static int timestpPrev = 0;
int timestpNow;
float height = 28;
bool state = 0;
// --- 用户自定义的应用 ---
// 这是一个简单的“关于”页面应用
class Penis : public IApplication {
private:
    PixelUI& m_ui;
public:
    Penis(PixelUI& ui) : m_ui(ui) {}
    void draw() override {
        // 假设可以这样访问UI对象
        U8G2Wrapper& display = m_ui.getU8G2();
        
        timestpNow = m_ui.getCurrentTime();

        if (timestpNow - timestpPrev > 100) {
            timestpPrev = timestpNow;
            if (state) {
                m_ui.animate(height, 35, 100, EasingType::LINEAR);
            } else {
                m_ui.animate(height, 28, 100, EasingType::LINEAR);
            }
            state = !state;
        }
        
        display.setFontMode(1);
        display.setBitmapMode(1);
        display.drawEllipse(69, 30, 4, 4);
        display.drawEllipse(54, 30, 4, 4);
        display.drawRFrame(58, 27, 8, height, 3);
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
};

// --- 自注册机制 ---
// 使用一个静态的 AppRegistrar 对象来在 main() 之前自动注册本应用
static AppRegistrar registrar_about_app({
    .title = "Penis Up Down",
    .bitmap = image_sans4_bits,
    
    // 关键点：提供一个创建 AboutApp 实例的工厂函数
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<Penis>(ui); 
    },
    
    .type = MenuItemType::App,
    .w = 24, 
    .h = 24
});