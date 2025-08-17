#include "PixelScriptUI/menu_system.h"

// 应用图标数据
static const uint8_t app_icon[] = { 0x01, 0x02, 0x03 };
static const uint8_t item_a_icon[] = { 0x0A };
static const uint8_t item_b_icon[] = { 0x0B };

// 应用菜单定义
static MenuLevel app_main_menu;
static MenuLevel app_sub_menu;

// 叶子节点动作函数
MenuLevel* itemAAction() {
    // 执行具体功能
    return nullptr; // 叶子节点
}

MenuLevel* itemBAction() {
    // 执行具体功能
    return nullptr; // 叶子节点
}

// 自动注册 - 在程序启动时自动执行
static MenuItemRegistrar item_a_reg(&app_sub_menu, {
    .title = "Function A",
    .bitmap = item_a_icon,
    .w = 16, .h = 16,
    .action = itemAAction
});

static MenuItemRegistrar item_b_reg(&app_sub_menu, {
    .title = "Function B", 
    .bitmap = item_b_icon,
    .w = 16, .h = 16,
    .action = itemBAction
});

static MenuItemRegistrar sub_menu_reg(&app_main_menu, {
    .title = "Sub Functions",
    .bitmap = app_icon,
    .w = 16, .h = 16,
    .action = []() -> MenuLevel* { return &app_sub_menu; }
});

// 注册为根菜单
static MenuLevelRegistrar app_root_reg(&app_main_menu, "My Application");