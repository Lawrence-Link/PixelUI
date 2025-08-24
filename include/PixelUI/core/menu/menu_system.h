#pragma once

#include <stdint.h>
#include <stddef.h>
#include <functional>

#define MAX_MENU_ITEMS 16
#define MAX_MENU_LEVEL 8
#define MAX_ROOT_MENUS 8

// 前向声明
class MenuLevel;

// 菜单项结构
struct MenuItem {
    const char* title;
    const uint8_t* bitmap;
    int w;
    int h;
    std::function<MenuLevel*()> action;
};

// 菜单级别结构
struct MenuLevel {
    MenuItem items[MAX_MENU_ITEMS];
    size_t itemCount;
    int selectedIndex;
    char title[32];
    
    MenuLevel() : itemCount(0), selectedIndex(0) {
        title[0] = '\0';
    }
};

// 菜单系统接口类
class IMenuSystem {
public:
    virtual ~IMenuSystem() = default;
    
    // 注册接口
    virtual bool registerRootMenu(MenuLevel* menu, const char* title = nullptr) = 0;
    virtual bool registerMenuItem(MenuLevel* parent, const MenuItem& item) = 0;
    
    // 导航接口
    virtual void setCurrentMenu(MenuLevel* menu) = 0;
    virtual MenuLevel* getCurrentMenu() const = 0;
    virtual const MenuItem* getCurrentItem() const = 0;
    
    virtual void navigateUp() = 0;
    virtual void navigateDown() = 0;
    virtual void selectItem() = 0;
    virtual void goBack() = 0;
    
    // 根菜单管理
    virtual MenuLevel* getRootMenu() = 0;
    virtual void buildRootMenu() = 0;
};

// 获取菜单系统单例
IMenuSystem& getMenuSystem();

// 自动注册辅助类
class MenuLevelRegistrar {
public:
    MenuLevelRegistrar(MenuLevel* menu, const char* title = nullptr);
};

class MenuItemRegistrar {
public:
    MenuItemRegistrar(MenuLevel* parent, const MenuItem& item);
};