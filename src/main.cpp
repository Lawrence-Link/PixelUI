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

U8G2Wrapper display;
PixelUI ui(display);

MainWindow* g_mainWindow = nullptr;

void threadDelay(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

class EmulatorThread : public EmuWorker {
public:
    void grandLoop() override { 
    ui.setDelayFunction(threadDelay);

    auto appView = std::make_shared<AppView>(ui, *ui.getViewManagerPtr());
    ui.getViewManagerPtr()->push(appView);
        while (running) {

            bool isDirty = ui.isDirty();
        
            auto eventOpt = g_mainWindow->popInputEvent();
            if (eventOpt.has_value()) {
                ui.handleInput(eventOpt.value());
            }
            // 只在内容改变时重绘
            if (isDirty) {
                ui.renderer();  // 调用 AppView::draw()
                emit updateRequested(); // 通知Qt更新
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

    ui.setRefreshCallback(refreshEmulator);

    worker.start();  // enable background thread for u8g2 emulation
    int ret = app.exec();
    worker.stop();   // stop the worker thread gracefully :)

    return ret;
}

