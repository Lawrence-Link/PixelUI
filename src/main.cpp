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
MainWindow* g_mainWindow = nullptr;
class EmulatorThread : public EmuWorker {
private:

void processInput() {
        if (!g_mainWindow) return;

        // 从队列中不断取出事件直到队列为空
        while(true) {
            
        }
    }

bool movingRight = false;

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
        
            auto eventOpt = g_mainWindow->popInputEvent();
            if (eventOpt.has_value()) {
                // 如果有事件，就交给PixelUI处理
                ui.handleInput(eventOpt.value());
            } else {
                // 队列为空，退出循环
                
            }

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
    g_mainWindow = &w;
    
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

