/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "u8g2_wrapper.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "QApplication"
#include <QTimer>

#include "MainWindow.h"
#include "EmuWorker.h"

#include "PixelUI.h"
#include "ui/AppLauncher/AppLauncher.h"

#include "app_registry.h"

U8G2Wrapper display;
PixelUI ui(display);

MainWindow* g_mainWindow = nullptr;

void threadDelay(uint32_t ms) {
    QThread::msleep(ms);
}

class EmulatorThread : public EmuWorker {
public:
    void grandLoop() override { 
    ui.setDelayFunction(threadDelay);

    #if (USE_STATIC_APP_REGISTER_ENABLED == 0) // If not using static app linking, register Apps manually
    registerApps();
    #endif

    ui.begin();
    auto appView = AppLauncher::createAppLauncherView(ui, *ui.getViewManagerPtr());
    ui.getViewManagerPtr()->push(etl::move(appView));
        while (running) {
        
            auto eventOpt = g_mainWindow->popInputEvent();
            if (eventOpt.has_value()) {
                ui.handleInput(eventOpt.value());
            }

            ui.renderer();
        
            QThread::msleep(16);
        }
    }
};


int main(int argc, char *argv[]) {
    puts("<<<<<< U8G2 Emulator - Qt Windowed Mode >>>>>>");

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

    VoidCallback refreshEmulator = [worker_ptr]() {
        emit worker_ptr->updateRequested();
    };

    ui.setRefreshCallback(refreshEmulator);

    worker.start();  // enable background thread for u8g2 emulation
    int ret = app.exec();
    worker.stop();   // stop the worker thread gracefully :)
    
    delete hbTimer;
    return ret;
}
