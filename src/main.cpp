#include <iostream>
#include "u8g2_wrapper.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "QApplication"
#include <QTimer>

#include "MainWindow.h"
#include "EmuWorker.h"

#include "PixelScriptUI/pixelscriptui.h"

// #include "PixelScriptUI/menu_system.h"
// #include "PixelScriptUI/app_example.h"

// #include "PixelScriptUI/PSUI_animation.h"

U8G2Wrapper display;
PixelUI ui(display);

// void renderMenu(const MenuLevel* menu) {
//     if (!menu) return;
    
//     std::cout << "\n=== " << menu->title << " ===\n";
//     for (size_t i = 0; i < menu->itemCount; ++i) {
//         std::cout << (i == menu->selectedIndex ? "> " : "  ");
//         std::cout << menu->items[i].title << "\n";
//     }
//     std::cout << "========================\n";
// }

class EmulatorThread : public EmuWorker {
private:

float X_LINEAR, Y_LINEAR = 0.0f;
float X_EASE_IN_QUAD, Y_EASE_IN_QUAD = 0.0f;
float X_EASE_OUT_QUAD, Y_EASE_OUT_QUAD = 0.0f;
float X_EASE_IN_OUT_QUAD, Y_EASE_IN_OUT_QUAD = 0.0f;
float X_EASE_IN_CUBIC, Y_EASE_IN_CUBIC = 0.0f;
float X_EASE_OUT_CUBIC, Y_EASE_OUT_CUBIC = 0.0f;
float X_EASE_OUT_BOUNCE, Y_EASE_OUT_BOUNCE = 0.0f;
float X_EASE_IN_OUT_CUBIC, Y_EASE_IN_OUT_CUBIC = 0.0f;


bool movingRight = false;
void startNextAnimation() {
        if (movingRight) {
            // 向右移动到80
            ui.animate(X_LINEAR,            Y_LINEAR,               111.0f, 1.0f,  1000, EasingType::LINEAR);
            ui.animate(X_EASE_IN_QUAD,      Y_EASE_IN_QUAD,         111.0f, 8.0f,  1000, EasingType::EASE_IN_QUAD);
            ui.animate(X_EASE_OUT_QUAD,     Y_EASE_OUT_QUAD,        111.0f, 15.0f, 1000, EasingType::EASE_OUT_QUAD);
            ui.animate(X_EASE_IN_OUT_QUAD,  Y_EASE_IN_OUT_QUAD,     111.0f, 22.0f, 1000, EasingType::EASE_IN_OUT_QUAD);
            ui.animate(X_EASE_IN_CUBIC,     Y_EASE_IN_CUBIC,        111.0f, 29.0f, 1000, EasingType::EASE_IN_CUBIC);
            ui.animate(X_EASE_OUT_CUBIC,    Y_EASE_OUT_CUBIC,       111.0f, 36.0f, 1000, EasingType::EASE_OUT_CUBIC);
            ui.animate(X_EASE_IN_OUT_CUBIC, Y_EASE_IN_OUT_CUBIC,    111.0f, 43.0f, 1000, EasingType::EASE_IN_OUT_CUBIC);
            ui.animate(X_EASE_OUT_BOUNCE,   Y_EASE_OUT_BOUNCE,      111.0f, 50.0f, 1000, EasingType::EASE_OUT_BOUNCE);
        } else {
            // 向左移动到20
            ui.animate(X_LINEAR,            Y_LINEAR,               10.0f, 1.0f,  1000, EasingType::LINEAR);
            ui.animate(X_EASE_IN_QUAD,      Y_EASE_IN_QUAD,         10.0f, 8.0f,  1000, EasingType::EASE_IN_QUAD);
            ui.animate(X_EASE_OUT_QUAD,     Y_EASE_OUT_QUAD,        10.0f, 15.0f, 1000, EasingType::EASE_OUT_QUAD);
            ui.animate(X_EASE_IN_OUT_QUAD,  Y_EASE_IN_OUT_QUAD,     10.0f, 22.0f, 1000, EasingType::EASE_IN_OUT_QUAD);
            ui.animate(X_EASE_IN_CUBIC,     Y_EASE_IN_CUBIC,        10.0f, 29.0f, 1000, EasingType::EASE_IN_CUBIC);
            ui.animate(X_EASE_OUT_CUBIC,    Y_EASE_OUT_CUBIC,       10.0f, 36.0f, 1000, EasingType::EASE_OUT_CUBIC);
            ui.animate(X_EASE_IN_OUT_CUBIC, Y_EASE_IN_OUT_CUBIC,    10.0f, 43.0f, 1000, EasingType::EASE_IN_OUT_CUBIC);
            ui.animate(X_EASE_OUT_BOUNCE,   Y_EASE_OUT_BOUNCE,      10.0f, 50.0f, 1000, EasingType::EASE_OUT_BOUNCE);
        }
    }

public:
    void grandLoop() override {
        // IMenuSystem& menuSystem = getMenuSystem();

        // // 设置根菜单为当前菜单
        // menuSystem.setCurrentMenu(menuSystem.getRootMenu());

        // char input;
        // std::cout << "Commands: u(up), d(down), s(select), b(back), q(quit)\n";

        // auto mainScene = std::make_shared<MainScene>();
        // gui.setScene(mainScene, TransitionType::ZOOM, 800);
    
    ui.begin(Screen::TileMenu);

    // ui.debugInfo();
    
    std::cout << "[TEST] Test completed" << std::endl;

        while (running) {   // the pseudo main loop for your u8g2 code to run in.

            
            // renderMenu(menuSystem.getCurrentMenu());
            // std::cout << "Command: ";
            // std::cin >> input;
        
            // switch (input) {
            //     case 'u': menuSystem.navigateUp(); break;
            //     case 'd': menuSystem.navigateDown(); break;
            //     case 's': menuSystem.selectItem(); break;
            //     case 'b': menuSystem.goBack(); break;
            //     default:
            //         break;
            // }
            
            if (ui.getActiveAnimationCount() == 0) {
                // 动画完成，切换方向并开始新动画
                movingRight = !movingRight;
                // std::this_thread::sleep_for(std::chrono::milliseconds(100));
                startNextAnimation();
            }

            display.clearBuffer();
            display.setFont(u8g2_font_tom_thumb_4x6_mf );
            display.setDrawColor(2);

            display.drawStr((128 - display.getStrWidth("Linear")) / 2, 6,  "Linear");
            display.drawStr((128 - display.getStrWidth("IN_QUAD")) / 2, 13, "IN_QUAD");
            display.drawStr((128 - display.getStrWidth("OUT_QUAD")) / 2, 20, "OUT_QUAD");
            display.drawStr((128 - display.getStrWidth("IN_OUT_QUAD")) / 2, 27, "IN_OUT_QUAD");
            display.drawStr((128 - display.getStrWidth("IN_CUBIC")) / 2, 34, "IN_CUBIC");
            display.drawStr((128 - display.getStrWidth("OUT_CUBIC")) / 2, 41, "OUT_CUBIC");
            display.drawStr((128 - display.getStrWidth("IN_OUT_CUBIC")) / 2, 48, "IN_OUT_CUBIC");
            display.drawStr((128 - display.getStrWidth("OUT_BOUNCE")) / 2, 54, "OUT_BOUNCE");

            display.drawBox(X_LINEAR,           Y_LINEAR,6,6);
            display.drawBox(X_EASE_IN_QUAD,     Y_EASE_IN_QUAD,6,6);
            display.drawBox(X_EASE_OUT_QUAD,    Y_EASE_OUT_QUAD,6,6);
            display.drawBox(X_EASE_IN_OUT_QUAD, Y_EASE_IN_OUT_QUAD,6,6);
            display.drawBox(X_EASE_IN_CUBIC,    Y_EASE_IN_CUBIC,6,6);
            display.drawBox(X_EASE_OUT_CUBIC,   Y_EASE_OUT_CUBIC,6,6);
            display.drawBox(X_EASE_IN_OUT_CUBIC,Y_EASE_IN_OUT_CUBIC,6,6);
            display.drawBox(X_EASE_OUT_BOUNCE,  Y_EASE_OUT_BOUNCE,6,6);
            display.sendBuffer();
            emit updateRequested();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};


int main(int argc, char *argv[]) {
    std::cout << "U8G2 Emulator - Qt Windowed Mode" << std::endl;

    QApplication app(argc, argv);
    display.init();

    // call this after u8g2.init(). otherwise the display size will be null
    MainWindow w(nullptr, display.getWidth(), display.getHeight(), 10); // initial size 128x64, scale factor 10
    
    w.show();

    EmulatorThread worker;
    QObject::connect(&worker, &EmulatorThread::updateRequested, &w, [&w]() {
        auto pixels = display.getFramebufferPixels();
        w.setPixels(pixels);
        w.update(); 
    });

    QTimer* hbTimer = new QTimer();
    QObject::connect(hbTimer, &QTimer::timeout, [&](){
        ui.Heartbeat(16);
    });

    hbTimer->start(16); // 16ms = ~60FPS

    // EmulatorThread *worker_ptr = &worker;

    // display.setUpdateCallback([worker_ptr]() {
    //     emit worker_ptr->updateRequested();
    // });

    worker.start();  // enable background thread for u8g2 emulation
    int ret = app.exec();
    worker.stop();   // stop the worker thread gracefully :)

    return ret;
}

