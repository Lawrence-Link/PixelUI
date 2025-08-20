#include <iostream>
#include "u8g2_wrapper.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "QApplication"
#include <QTimer>

#include "MainWindow.h"
#include "EmuWorker.h"

#include "PixelUI/pixelui.h"
#include "PixelUI/core/ui/AppView/Appview.h"

// #include "PixelUI/menu_system.h"
// #include "PixelUI/app_example.h"

// #include "PixelUI/PSUI_animation.h"

U8G2Wrapper display;
PixelUI ui(display);
ViewManager viewManager(ui);
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
    
    AppManager& manager = AppManager::getInstance();
    const auto& apps = manager.getAppVector();

    std::cout << "\nAppList:" << std::endl;
    for (const auto& app : apps) {
        std::cout << "------------------------" << std::endl;
        std::cout << "Title: " << app.title << std::endl;
        std::cout << "Type: ";
        switch(app.type) {
            case MenuItemType::App: std::cout << "App"; break;
        }
        std::cout << std::endl;
        std::cout << "Size: " << app.w << "x" << app.h << std::endl;
        // 调用App的动作函数
        // if (app.action) {
        //     std::cout << "execute action: \n";
        //     app.action();
        // }
    }
    ui.begin();

    // ui.debugInfo();
    // float W_OUT_CUBIC, H_OUT_CUBIC = 10.0f;
    // ui.animate(W_OUT_CUBIC, H_OUT_CUBIC, 82.0f, 30.0f, 560, EasingType::EASE_IN_OUT_CUBIC);
    
    auto appView = std::make_shared<AppView>(ui, viewManager);
    
    viewManager.push(appView);

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
            
            // if (ui.getActiveAnimationCount() == 0) {
            //     // 动画完成，切换方向并开始新动画
            //     movingRight = !movingRight;
            //     startNextAnimation();
            // }

            bool isDirty = ui.isDirty();
        
        // 只在内容改变时重绘
            if (isDirty) {
                display.clearBuffer();
                ui.renderer();  // 调用 AppView::draw()
                display.sendBuffer();  // 发送到屏幕
                emit updateRequested(); // 通知Qt更新
                
                std::cout << "[DEBUG] Frame rendered and sent to display" << std::endl;
            }
        
        // 检查动画是否在运行，如果有动画则持续标记为dirty
            if (ui.getActiveAnimationCount() > 0) {
                ui.markDirty();
            }
        
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
};


int main(int argc, char *argv[]) {
    std::cout << "U8G2 Emulator - Qt Windowed Mode" << std::endl;

    QApplication app(argc, argv);
    display.init();

    // call this after u8g2.init(). otherwise the display size will be null
    MainWindow w(nullptr, display.getWidth(), display.getHeight(), 3); // initial size 128x64, scale factor 10
    
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

