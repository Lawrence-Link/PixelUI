/*
 * Copyright (C) 2025 Lawrence Li
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
    ui.begin();
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

