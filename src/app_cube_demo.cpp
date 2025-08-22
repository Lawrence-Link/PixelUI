#include "PixelUI/core/app/IApplication.h"  // App接口
#include "PixelUI/core/app/app_system.h"    // App系统
#include <memory>
#include <iostream>

static const unsigned char image_sans2_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0x7f,0x00,0xff,0x9f,0xff,0xfc,0xef,0xff,0xfb,0xef,0xff,0xfb,0xf7,0xff,0xf7,0xf7,0x3f,0xf6,0xf7,0xbf,0xf6,0x37,0x36,0xf6,0xef,0xe3,0xfb,0xa7,0xff,0xf2,0x37,0x00,0xf6,0x77,0x55,0xf7,0xcf,0x80,0xf9,0x1f,0x7f,0xfc,0x7f,0x00,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};


float cube_vertices[8][3] = {
    {-0.3, -0.3, -0.3},
    { 0.3, -0.3, -0.3},
    { 0.3,  0.3, -0.3},
    {-0.3,  0.3, -0.3},
    {-0.3, -0.3,  0.3},
    { 0.3, -0.3,  0.3},
    { 0.3,  0.3,  0.3},
    {-0.3,  0.3,  0.3}
};

uint8_t cube_edges[12][2] = {
    {0,1},{1,2},{2,3},{3,0}, // 底面
    {4,5},{5,6},{6,7},{7,4}, // 顶面
    {0,4},{1,5},{2,6},{3,7}  // 竖线
};


void rotate3D(float v[3], float angleX, float angleY) {
    float x = v[0], y = v[1], z = v[2];
    float sinX = sin(angleX), cosX = cos(angleX);
    float sinY = sin(angleY), cosY = cos(angleY);

    // 绕 X 轴旋转
    float y1 = y*cosX - z*sinX;
    float z1 = y*sinX + z*cosX;
    y = y1; z = z1;

    // 绕 Y 轴旋转
    float x1 = x*cosY + z*sinY;
    float z2 = -x*sinY + z*cosY;
    v[0] = x1; v[1] = y; v[2] = z2;
}

void project2D(float v[3], int16_t &x2d, int16_t &y2d, int16_t screenW, int16_t screenH) {
    float fov = 64;       // 视角因子
    float distance = 17.0; // 观察点距离
    float factor = fov / (v[2] + distance);

    x2d = static_cast<int16_t>(screenW/2 + v[0] * factor * screenW/16);
    y2d = static_cast<int16_t>(screenH/2 - v[1] * factor * screenH/16);
}

void drawCube(U8G2 &display, float angleX, float angleY) {
    float transformed[8][3];
    int16_t projected[8][2];

    for (int i = 0; i < 8; i++) {
        // 拷贝顶点
        transformed[i][0] = cube_vertices[i][0];
        transformed[i][1] = cube_vertices[i][1];
        transformed[i][2] = cube_vertices[i][2];

        // 旋转
        rotate3D(transformed[i], angleX, angleY);

        // 投影
        project2D(transformed[i],
                  projected[i][0], projected[i][1],
                  128,
                  64);
    }

    // 画 12 条边
    for (int i = 0; i < 12; i++) {
        int a = cube_edges[i][0];
        int b = cube_edges[i][1];
        display.drawLine(projected[a][0], projected[a][1],
                         projected[b][0], projected[b][1]);
    }
}

static float angleX = 0, angleY = 0;
static float height = 28;
static bool state = 0;

// --- 用户自定义的应用 ---
// 这是一个简单的“关于”页面应用
class AboutApp2 : public IApplication {
public:
    void draw() override {
        extern PixelUI ui;
        ui.markDirty();
        U8G2Wrapper& display = ui.getU8G2();
        
        display.setFont(u8g2_font_ncenB10_tr);
        display.drawStr(20, 20, "Cube Demo");

        drawCube(display, angleX, angleY);
        angleX += 0.05f;
        angleY += 0.03f;
        
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
        std::cout << "[AboutApp] Entered." << std::endl;
    }

    void onExit() {
        extern PixelUI ui;
        ui.setContinousDraw(false);
        ui.markFading();
    }
};

static AppRegistrar registrar_about_app({
    .title = "Cube Demo",
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