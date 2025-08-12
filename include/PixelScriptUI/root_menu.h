#pragma once
#ifndef ROOT_MENU_H
#define ROOT_MENU_H

#include "menu_manager.h"

// 声明虚拟的根菜单，包含所有自注册的菜单
extern MenuLevel root_menu_list;

// 声明构建根菜单的函数
void build_root_menu();

#endif // ROOT_MENU_H