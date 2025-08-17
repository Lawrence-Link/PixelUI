#include "PixelScriptUI/core/menu/menu_system.h"
#include <string.h>

class MenuSystemImpl : public IMenuSystem {
private:
    MenuLevel* mCurrent;
    MenuLevel* mStack[MAX_MENU_LEVEL];
    int mDepth;
    
    MenuLevel* mRootMenus[MAX_ROOT_MENUS];
    size_t mRootCount;
    
    MenuLevel mVirtualRoot;  // 虚拟根菜单
    bool mRootBuilt;

public:
    MenuSystemImpl() : mCurrent(nullptr), mDepth(0), mRootCount(0), mRootBuilt(false) {
        memset(mStack, 0, sizeof(mStack));
        memset(mRootMenus, 0, sizeof(mRootMenus));
        strcpy(mVirtualRoot.title, "<AppList>");
    }
    
    bool registerRootMenu(MenuLevel* menu, const char* title = nullptr) override {
        if (!menu || mRootCount >= MAX_ROOT_MENUS) {
            return false;
        }
        
        if (title) {
            strncpy(menu->title, title, sizeof(menu->title) - 1);
            menu->title[sizeof(menu->title) - 1] = '\0';
        }
        
        mRootMenus[mRootCount++] = menu;
        mRootBuilt = false; // 标记需要重建根菜单
        return true;
    }
    
    bool registerMenuItem(MenuLevel* parent, const MenuItem& item) override {
        if (!parent || parent->itemCount >= MAX_MENU_ITEMS) {
            return false;
        }
        
        parent->items[parent->itemCount++] = item;
        return true;
    }
    
    void buildRootMenu() override {
        if (mRootBuilt) return;
        
        mVirtualRoot.itemCount = 0;
        mVirtualRoot.selectedIndex = 0;
        
        for (size_t i = 0; i < mRootCount && mVirtualRoot.itemCount < MAX_MENU_ITEMS; ++i) {
            MenuLevel* rootMenu = mRootMenus[i];
            if (!rootMenu) continue;
            
            // 为每个根菜单创建一个入口项
            mVirtualRoot.items[mVirtualRoot.itemCount++] = {
                .title = rootMenu->title,
                .bitmap = nullptr, // 可以设置默认图标
                .w = 16,
                .h = 16,
                .action = [rootMenu]() -> MenuLevel* {
                    return rootMenu;
                }
            };
        }
        
        mRootBuilt = true;
    }
    
    MenuLevel* getRootMenu() override {
        buildRootMenu();
        return &mVirtualRoot;
    }
    
    void setCurrentMenu(MenuLevel* menu) override {
        if (menu) {
            mCurrent = menu;
            mDepth = 0;
        }
    }
    
    MenuLevel* getCurrentMenu() const override {
        return mCurrent;
    }
    
    const MenuItem* getCurrentItem() const override {
        if (!mCurrent || mCurrent->itemCount == 0) {
            return nullptr;
        }
        
        int idx = mCurrent->selectedIndex;
        if (idx >= 0 && idx < static_cast<int>(mCurrent->itemCount)) {
            return &mCurrent->items[idx];
        }
        
        return nullptr;
    }
    
    void navigateUp() override {
        if (!mCurrent || mCurrent->itemCount == 0) return;
        
        mCurrent->selectedIndex--;
        if (mCurrent->selectedIndex < 0) {
            mCurrent->selectedIndex = static_cast<int>(mCurrent->itemCount) - 1;
        }
    }
    
    void navigateDown() override {
        if (!mCurrent || mCurrent->itemCount == 0) return;
        
        mCurrent->selectedIndex++;
        if (mCurrent->selectedIndex >= static_cast<int>(mCurrent->itemCount)) {
            mCurrent->selectedIndex = 0;
        }
    }
    
    void selectItem() override {
        const MenuItem* item = getCurrentItem();
        if (item && item->action) {
            MenuLevel* nextLevel = item->action();
            if (nextLevel) {
                pushStack(mCurrent);
                mCurrent = nextLevel;
                mCurrent->selectedIndex = 0;
            }
        }
    }
    
    void goBack() override {
        MenuLevel* prevLevel = popStack();
        if (prevLevel) {
            mCurrent = prevLevel;
        }
    }

private:
    void pushStack(MenuLevel* menu) {
        if (mDepth < MAX_MENU_LEVEL) {
            mStack[mDepth++] = menu;
        }
    }
    
    MenuLevel* popStack() {
        if (mDepth > 0) {
            return mStack[--mDepth];
        }
        return nullptr;
    }
};

// 全局单例实例
IMenuSystem& getMenuSystem() {
    static MenuSystemImpl instance;
    return instance;
}

// 自动注册实现
MenuLevelRegistrar::MenuLevelRegistrar(MenuLevel* menu, const char* title) {
    getMenuSystem().registerRootMenu(menu, title);
}

MenuItemRegistrar::MenuItemRegistrar(MenuLevel* parent, const MenuItem& item) {
    getMenuSystem().registerMenuItem(parent, item);
}