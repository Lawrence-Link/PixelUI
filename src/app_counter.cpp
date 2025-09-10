// app_example.cpp (或者任何用户自定义的 app_*.cpp 文件)
#include "PixelUI/core/app/IApplication.h"
#include "PixelUI/core/app/app_system.h"
#include <memory>
#include <iostream>
#include <etl/stack.h>
#include "PixelUI/core/widgets/histogram/histogram.h"
#include "PixelUI/core/widgets/brace/brace.h"

static const unsigned char image_info_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0xff,0x00,0xff,0x7f,0x3e,0xff,0x7f,0x3f,0xff,0xff,0x3f,0xff,0xff,0x1f,0xff,0xff,0x8f,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};

static const unsigned char image_Background_bits[] = {0xfe,0x01,0x00,0x00,0x00,0x00,0x00,0xe0,0xff,0xff,0xff,0x0f,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x7d,0x06,0x00,0x00,0x00,0x00,0x00,0x18,0xff,0xb7,0x55,0x31,0x00,0x00,0x00,0x00,0x81,0xfc,0xff,0xff,0xff,0xff,0xff,0x8f,0x00,0x00,0x00,0xe2,0xff,0xff,0xff,0x7f,0x3d,0x01,0x00,0x00,0x00,0x00,0x00,0x40,0xb6,0xea,0xff,0x04,0x00,0x00,0x00,0x80,0x41,0xfe,0xff,0xff,0xaa,0xfe,0xff,0x3f,0x01,0x00,0x00,0xf9,0xff,0xff,0xff,0xab,0x9f,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xf8,0xff,0x7f,0x02,0x00,0x00,0x00,0x80,0x3e,0xff,0xff,0xff,0xff,0x55,0xfd,0x7f,0xfc,0xff,0xff,0x6c,0xff,0xff,0xff,0xb5,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x80,0x01,0x00,0x00,0x00,0x80,0xc0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0x80,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x01,0x00,0x00,0xfe,0xff,0xff,0xff,0x7f};

static float s_static_data_buffer[25] = {
    0.1f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 0.9f, 0.7f, 0.5f, 0.3f,
    0.2f, 0.1f, 0.3f, 0.5f, 0.7f, 0.9f, 1.0f, 0.8f, 0.6f, 0.4f,
    0.2f, 0.1f, 0.2f, 0.3f, 0.4f
};

class APP_COUNTER: public IApplication {
private:
    PixelUI& m_ui;
    Histogram histogram;
    Brace brace;

    // 状态机相关
    enum class LoadState {
        INIT,           // 初始
        BRACE_LOADING,  // 执行brace.onLoad()
        WAIT_HISTO,     // 等待300ms
        HISTO_LOADING,  // 执行histogram.onLoad()
        DONE            // 全部完成
    } loadState = LoadState::INIT;

    uint32_t state_timestamp = 0;  // 记录状态切换时间戳
    bool first_time = false;

    // 动画变量
    float anim_mark_m = 0;
    float anim_bg = 0;
    float anim_status_x = -27;
public:
    APP_COUNTER(PixelUI& ui) : m_ui(ui), histogram(ui), brace(ui) {}

    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);

        // HISTOGRAM
        histogram.setCoordinate(97,54);
        histogram.setMargin(56,18);
        
        // BRACE 
        brace.setCoordinate(31,54);
        brace.setMargin(56,18);
        brace.setDrawContentFunction([this]() { braceContent(); });

        loadState = LoadState::INIT;
        first_time = false;
    }

    void braceContent() {
        U8G2& u8g2 = m_ui.getU8G2();
        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(30, 54, "10.00");
        u8g2.drawStr(31, 61, "uSv/h");
        u8g2.drawRBox(8, 50, 20, 10, 2);
        u8g2.setDrawColor(0);
        u8g2.drawStr(11, 58, "Max");
        u8g2.setDrawColor(1);
    }

    void draw() override {
        if (!first_time) {
            // 启动进度动画
            m_ui.animate(anim_mark_m, 23, 300, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
            m_ui.animate(anim_bg, 128, 500, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
            
            // 状态机从BRACE开始
            loadState = LoadState::BRACE_LOADING;
            state_timestamp = m_ui.getCurrentTime();
            first_time = true;
        }

        // 状态机驱动
        switch (loadState) {
            case LoadState::BRACE_LOADING:
                brace.onLoad();
                loadState = LoadState::WAIT_HISTO;
                state_timestamp = m_ui.getCurrentTime(); // 记录brace完成的时间
                break;

            case LoadState::WAIT_HISTO:
                if (m_ui.getCurrentTime() - state_timestamp >= 80) {
                    loadState = LoadState::HISTO_LOADING;
                }
                break;

            case LoadState::HISTO_LOADING:
                histogram.onLoad();
                loadState = LoadState::DONE;
                m_ui.animate(anim_status_x, 29, 450, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
                break;

            case LoadState::DONE:
                // 什么都不做
                break;

            default:
                break;
        }

        // --- UI绘制 ---
        U8G2& u8g2 = m_ui.getU8G2();
        u8g2.setClipWindow(0,5,anim_bg,15);
        u8g2.drawXBM(0, 5, 128, 11, image_Background_bits);
        u8g2.setMaxClipWindow();
        
        histogram.setData(s_static_data_buffer, 25, 0);
        histogram.draw();
        brace.draw();

        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(5, 42, "MEAS");

        u8g2.setClipWindow(29,36,83,42);
        u8g2.drawStr(anim_status_x, 42, "PLEASE WAIT");
        u8g2.setMaxClipWindow();

        u8g2.setDrawColor(2);
        u8g2.drawBox(3, 35, anim_mark_m, 8);
        u8g2.setDrawColor(1);
        u8g2.setFont(u8g2_font_profont17_tr);
        u8g2.drawStr(3, 31, "0.25uSv/h");

        u8g2.setFont(u8g2_font_4x6_tr);
        u8g2.drawStr(100, 32, "CNT");
        u8g2.drawStr(100, 39, "1234");
    }

    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit();
            return true;
        }
        return false;
    }

    void onExit() {
        extern PixelUI ui;
        ui.markFading();
    }
};


static AppRegistrar registrar_about_app({
    .title = "COUNTER",
    .bitmap = image_info_bits,
    
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<APP_COUNTER>(ui); 
    },
    
    .type = MenuItemType::App,
});