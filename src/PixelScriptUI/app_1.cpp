#include "PixelScriptUI/menu_manager.h"

// 模拟的位图数据，在实际项目中，这可能是FLASH中的数据
static const uint8_t app1_icon_bitmap[] = { 0x01 };
static const uint8_t app1_sub_menu_icon_bitmap[] = { 0x02 };
static const uint8_t app1_item_a_bitmap[] = { 0x0A };
static const uint8_t app1_item_b_bitmap[] = { 0x0B };

//
// 1. 声明菜单级别
//
// 这些是全局静态变量，它们的初始化顺序并不确定。
// 使用 .items = {}, .itemCount = 0 等语法来确保它们被正确初始化。
static MenuLevel app1_main_menu = {
    .items = {},
    .itemCount = 0,
    .selectedIndex = 0
};

static MenuLevel app1_sub_menu = {
    .items = {},
    .itemCount = 0,
    .selectedIndex = 0
};

//
// 2. 声明和注册菜单项
//
// 这些 MenuRegistrar 的全局静态对象将在 main() 函数执行前被构造。
// 它们会利用 MenuManager 的自注册功能，将菜单项链接到对应的父菜单。

// 注册 "Item A" 到 app1_sub_menu
static MenuItemRegistrar item_a_registrar(
    &app1_sub_menu, // this is the parent menu
    {               // these are the menu item properties
        .title = "App1 Item A",
        .bitmap = app1_item_a_bitmap,
        .w = 16,
        .h = 16,
        .action = nullptr // 这是叶子节点，没有子菜单，所以 action 为空
    }
);

// 注册 "Item B" 到 app1_sub_menu
static MenuItemRegistrar item_b_registrar(
    &app1_sub_menu,
    {
        .title = "App1 Item B",
        .bitmap = app1_item_b_bitmap,
        .w = 16,
        .h = 16,
        .action = nullptr
    }
);

//
// 3. 注册主菜单级别中的菜单项
//
// 这是一个跳转到子菜单的菜单项
static MenuItemRegistrar sub_menu_item_registrar(
    &app1_main_menu,
    {
        .title = "Sub Menu Main",
        .bitmap = app1_sub_menu_icon_bitmap,
        .w = 16,
        .h = 16,
        .action = []() -> MenuLevel* {
            // 这个 action 返回 app1_sub_menu 的地址
            return &app1_sub_menu;
        }
    }
);

//
// 4. 注册 app1_main_menu 为一个根菜单
//
// 这是将 app1 的主菜单作为整个菜单系统的顶级入口之一进行注册
static MenuLevelRegistrar app1_root_registrar(&app1_main_menu);