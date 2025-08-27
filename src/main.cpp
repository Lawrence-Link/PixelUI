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
#include "PixelUI/core/ui/AppView/AppView.h"

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
public:
    void grandLoop() override { 
    AppManager& manager = AppManager::getInstance();
    const auto& apps = manager.getAppVector();
    ui.begin();

    auto appView = std::make_shared<AppView>(ui, viewManager);
    
    viewManager.push(appView);

        while (running) {

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
                
                ui.renderer();  // 调用 AppView::draw()
                emit updateRequested(); // 通知Qt更新
                #ifdef DEBUG
                std::cout << "[DEBUG] Frame rendered and sent to display" << std::endl;
                #endif
            }
        
        // 检查动画是否在运行，如果有动画则持续标记为dirty
            if (ui.getActiveAnimationCount() > 0 || ui.isContinousRefreshEnabled()) {
                ui.markDirty();
            }
        
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
};


int main(int argc, char *argv[]) {
    std::cout << "<<<<<< U8G2 Emulator - Qt Windowed Mode >>>>>>" << std::endl;

    QApplication app(argc, argv);
    display.init();

    // call this after u8g2.init(). otherwise the display size will be null
    MainWindow w(nullptr, display.getWidth(), display.getHeight(), 5); // initial size 128x64, scale factor 10
    
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

    EmulatorThread *worker_ptr = &worker;

    // display.setUpdateCallback([worker_ptr]() {
    //     emit worker_ptr->updateRequested();
    // });

    std::function<void()> refreshEmulator = [worker_ptr]() {
        emit worker_ptr->updateRequested();
    };

    ui.setEmuRefreshFunc(refreshEmulator);

    worker.start();  // enable background thread for u8g2 emulation
    int ret = app.exec();
    worker.stop();   // stop the worker thread gracefully :)

    return ret;
}

