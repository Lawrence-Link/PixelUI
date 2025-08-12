#include <iostream>
#include "u8g2_wrapper.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "QApplication"
#include "MainWindow.h"
#include "EmuWorker.h"
#include "PixelScriptUI/menu_manager.h"
#include "PixelScriptUI/root_menu.h"
#include "PixelScriptUI/menu_manager.h"

U8G2Wrapper u8g2; // U8G2 wrapper instance, inherits from U8G2, use it like you would use U8G2 directly.

void renderMenu(const MenuLevel* menu) {
    if (!menu) {
        printf("No menu to render.\n");
        return;
    }

    printf("------------------------------\n");
    printf("         Current Menu         \n");
    printf("------------------------------\n");

    for (size_t i = 0; i < menu->itemCount; ++i) {
        // 获取菜单项的指针
        const MenuItem* item = &menu->items[i];
        
        // 打印选中标记
        if (i == menu->selectedIndex) {
            printf("> ");
        } else {
            printf("  ");
        }
        
        // 打印菜单项的标题
        if (item->title) {
            printf("%s\n", item->title);
        } else {
            printf("Unnamed Item\n");
        }
    }
    printf("------------------------------\n");
    printf("\n");
}

class EmulatorThread : public EmuWorker {
public:
    void grandLoop() override {
        build_root_menu();
        MenuManager& manager = MenuManager::instance();
        manager.setStartupRoot(&root_menu_list);
        char input;
        printf("Use 'u' for Up, 'd' for Down, 's' for Select, 'b' for Back, 'q' for Quit.\n\n");

        while (running) {   // the pseudo main loop for your u8g2 code to run in.
            
            renderMenu(manager.getCurrentMenuLevel());
        printf("Enter command: ");
        
        // 模拟从标准输入获取按键事件
        scanf(" %c", &input); // 注意空格，用于跳过换行符
        
        if (input == 'q') {
            break; // 退出循环
        }
        
        switch (input) {
            case 'u':
                manager.navigateUp();
                break;
            case 'd':
                manager.navigateDown();
                break;
            case 's':
                manager.selectItem();
                break;
            case 'b':
                manager.goBack();
                break;
            default:
                printf("Invalid command.\n");
                break;
        }
            
            u8g2.clearBuffer();

            u8g2.sendBuffer();
            emit updateRequested();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
};


int main(int argc, char *argv[]) {
    std::cout << "U8G2 Emulator - Qt Windowed Mode" << std::endl;

    QApplication app(argc, argv);
    u8g2.init();

    // call this after u8g2.init(). otherwise the display size will be null
    MainWindow w(nullptr, u8g2.getWidth(), u8g2.getHeight(), 10); // initial size 128x64, scale factor 10
    
    w.show();

    EmulatorThread worker;
    QObject::connect(&worker, &EmulatorThread::updateRequested, &w, [&w]() {
        auto pixels = u8g2.getFramebufferPixels();
        w.setPixels(pixels);
        w.update(); 
    });

    worker.start();  // enable background thread for u8g2 emulation
    int ret = app.exec();
    worker.stop();   // stop the worker thread gracefully :)

    return ret;
}

