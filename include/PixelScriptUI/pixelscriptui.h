#pragma once

#include "QApplication"
#include "MainWindow.h"
#include "EmuWorker.h"
#include "menu_manager.h"
#include <iostream>
#include "u8g2_wrapper.h"

class UIManager {
public:
    static UIManager& instance() {
        static UIManager inst;
        return inst;
    }
    void init();
    void loop();
    // Renderer& renderer() { return _renderer; }
    // EventDispatcher& events() { return _events; }
private:
    UIManager() {}
};