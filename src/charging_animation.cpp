#include "PixelUI/core/app/IApplication.h"  // App接口
#include "PixelUI/core/app/app_system.h"    // App系统
#include <memory>
#include <iostream>

static const unsigned char image_sans2_bits[] = {
0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0x3f,0x00,0xff,0x3f,0x7f,0xfe,0xbf,0x7e,0xfd,0xbf,0x00,0xfc,0xbf,0x7e,0xfd,0xbf,0x72,0xfd,0xbf,0x6a,0xfd,0xbf,0x72,0xfd,0xbf,0x6a,0xfd,0x3f,0x00,0xfd,0x7f,0xfe,0xec,0xf3,0x00,0xc4,0xe7,0xff,0xef,0xcf,0xff,0xe3,0x1f,0xff,0xf8,0x7e,0x3c,0x7e,0xfe,0x81,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};


class ChargeDemo : public IApplication {
private:
    PixelUI& m_ui;

void drawChargingLightning(int size) {
    // 获取屏幕尺寸
    int screenWidth = 128;   
    int screenHeight = 64;
    
    // 计算屏幕中心点
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    
    // 正确的对称闪电设计 - 上尖端在右，下尖端在左
    int p1x = centerX + size * 0.4;         // 右上尖端
    int p1y = centerY - size * 0.6;
    
    int p2x = centerX - size * 0.1;         // 左上内折点
    int p2y = centerY - size * 0.1;
    
    int p3x = centerX + size * 0.25;        // 右中突出点
    int p3y = centerY - size * 0.1;
    
    int p4x = centerX - size * 0.25;        // 左中突出点
    int p4y = centerY + size * 0.1;
    
    int p5x = centerX + size * 0.1;         // 右下内折点
    int p5y = centerY + size * 0.1;
    
    int p6x = centerX - size * 0.4;         // 左下尖端
    int p6y = centerY + size * 0.6;
    
    // 绘制充电闪电图标
    m_ui.getU8G2().drawLine(p1x, p1y, p2x, p2y);  // 右上尖端到左上内折
    m_ui.getU8G2().drawLine(p2x, p2y, p3x, p3y);  // 左上内折到右中突出
    m_ui.getU8G2().drawLine(p3x, p3y, p4x, p4y);  // 中间的Z字连接线
    m_ui.getU8G2().drawLine(p4x, p4y, p5x, p5y);  // 左中突出到右下内折
    m_ui.getU8G2().drawLine(p5x, p5y, p6x, p6y);  // 右下内折到左下尖端
    m_ui.getU8G2().drawLine(p6x, p6y, p1x, p1y);  // 回到起点（闭合）
}

public:
    ChargeDemo(PixelUI& ui):m_ui(ui) {};
    void draw() override {
        m_ui.markDirty();
        U8G2Wrapper& display = m_ui.getU8G2();
        drawChargingLightning(10);
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
        extern PixelUI ui;
        ui.setContinousDraw(true);
        ui.markDirty(); 
    }

    void onExit() {
        extern PixelUI ui;
        ui.setContinousDraw(false);
        ui.markFading();
    }
};

static AppRegistrar registrar_about_app({
    .title = "Lighting Demo",
    .bitmap = image_sans2_bits,
    
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<ChargeDemo>(ui); 
    },
    
    .type = MenuItemType::App,
    .w = 24, 
    .h = 24,
    .useUnifiedEnterAnimation = true
});