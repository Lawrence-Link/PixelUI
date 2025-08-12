#pragma once
// menu_manager.h
#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <functional> // 引入 std::function

#define MAX_MENU_ITEMS 16
#define MAX_MENU_LEVEL 8
#define MAX_ROOT_MENUS 8

class MenuLevel;
class MenuManager;

struct MenuItem {
    const char* title;
    const uint8_t* bitmap;
    int w;
    int h;
    // 使用 std::function 来存储可调用对象，可以捕获变量
    std::function<MenuLevel*()> action; 
};

struct MenuLevel {
    MenuItem items[MAX_MENU_ITEMS];
    size_t itemCount;
    int selectedIndex;
};

class MenuLevelRegistrar;
class MenuItemRegistrar;

void build_root_menu();

class MenuManager {
public:
    static MenuManager& instance() {
        static MenuManager s;
        return s;
    }

    void registerRoot(MenuLevel* root) {
        if (root && mRootCount < MAX_ROOT_MENUS) {
            mRootList[mRootCount++] = root;
        }
    }

    void registerItem(MenuLevel* parent, const MenuItem& item) {
        if (parent && parent->itemCount < MAX_MENU_ITEMS) {
            parent->items[parent->itemCount++] = item;
        }
    }

    void setStartupRoot(MenuLevel* root) {
        mCurrent = root;
        mDepth = 0;
        mStack[0] = nullptr;
    }

    MenuLevel* getCurrentMenuLevel() const {
        return mCurrent;
    }
    
    const MenuItem* getCurrentItem() const {
        if (mCurrent && mCurrent->itemCount > 0 && mCurrent->selectedIndex >= 0 && mCurrent->selectedIndex < mCurrent->itemCount) {
            return &mCurrent->items[mCurrent->selectedIndex];
        }
        return nullptr;
    }

    void selectItem() {
        if (mCurrent) {
            const MenuItem* currentItem = getCurrentItem();
            if (currentItem && currentItem->action) {
                MenuLevel* nextLevel = currentItem->action();
                if (nextLevel) {
                    pushStack(mCurrent);
                    mCurrent = nextLevel;
                    mCurrent->selectedIndex = 0;
                }
            }
        }
    }

    void goBack() {
        MenuLevel* prevLevel = popStack();
        if (prevLevel) {
            mCurrent = prevLevel;
        }
    }

    void navigateUp() {
        if (mCurrent && mCurrent->itemCount > 0) {
            mCurrent->selectedIndex--;
            if (mCurrent->selectedIndex < 0) {
                mCurrent->selectedIndex = mCurrent->itemCount - 1;
            }
        }
    }

    void navigateDown() {
        if (mCurrent && mCurrent->itemCount > 0) {
            mCurrent->selectedIndex++;
            if (mCurrent->selectedIndex >= mCurrent->itemCount) {
                mCurrent->selectedIndex = 0;
            }
        }
    }

private:
    MenuManager();

    MenuLevel* popStack();
    void pushStack(MenuLevel* n);

    MenuLevel* mCurrent;
    MenuLevel* mStack[MAX_MENU_LEVEL];
    int mDepth;

    MenuLevel* mRootList[MAX_ROOT_MENUS];
    size_t mRootCount;
    
    MenuManager(const MenuManager&) = delete;
    MenuManager& operator=(const MenuManager&) = delete;
    
    friend void build_root_menu();
};

class MenuLevelRegistrar {
public:
    MenuLevelRegistrar(MenuLevel* root) {
        MenuManager::instance().registerRoot(root);
    }
};

class MenuItemRegistrar {
public:
    MenuItemRegistrar(MenuLevel* parent, const MenuItem& item) {
        MenuManager::instance().registerItem(parent, item);
    }
};

#endif // MENU_MANAGER_H