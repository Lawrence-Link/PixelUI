#include "PixelScriptUI/root_menu.h"
#include <string.h>

MenuLevel root_menu_list = {
    .items = {},
    .itemCount = 0,
    .selectedIndex = 0
};

void build_root_menu() {
    MenuManager& manager = MenuManager::instance();
    
    memset(&root_menu_list, 0, sizeof(MenuLevel));

    for (size_t i = 0; i < manager.mRootCount; ++i) {
        if (root_menu_list.itemCount < MAX_MENU_ITEMS) {
            MenuLevel* rootLevel = manager.mRootList[i];
            
            if (rootLevel && rootLevel->itemCount > 0) {
                const MenuItem* rootItem = &rootLevel->items[0];
                
                // 使用 lambda 表达式，并捕获 rootLevel 变量
                // std::function 可以完美地存储这个 lambda
                root_menu_list.items[root_menu_list.itemCount++] = {
                    .title = rootItem->title,
                    .bitmap = rootItem->bitmap,
                    .w = rootItem->w,
                    .h = rootItem->h,
                    .action = [rootLevel]() -> MenuLevel* {
                        return rootLevel;
                    }
                };
            }
        }
    }
}