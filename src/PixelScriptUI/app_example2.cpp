// app_example2.cpp
#include "PixelScriptUI/menu_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 计算器图标位图数据
static const uint8_t calculator_icon_bitmap[] = { 0x10, 0x11, 0x12 };
static const uint8_t add_icon_bitmap[] = { 0x2B }; // '+'
static const uint8_t sub_icon_bitmap[] = { 0x2D }; // '-'
static const uint8_t mul_icon_bitmap[] = { 0x2A }; // '*'
static const uint8_t div_icon_bitmap[] = { 0x2F }; // '/'

// 计算器状态
static struct {
    double operand1;
    double operand2;
    char operation;
    bool has_operand1;
    bool has_operand2;
    double result;
    bool has_result;
} calc_state = {0, 0, 0, false, false, 0, false};

// 声明菜单级别 - 使用默认构造函数
static MenuLevel calculator_main_menu;
static MenuLevel calculator_operations_menu;

// 简单的输入函数（在实际嵌入式系统中，这里会是按键输入）
static double getNumberInput(const char* prompt) {
    double value;
    printf("%s", prompt);
    if (scanf("%lf", &value) == 1) {
        return value;
    }
    return 0.0;
}

// 计算器操作函数
static MenuLevel* performAddition() {
    printf("\n=== 加法运算 ===\n");
    calc_state.operand1 = getNumberInput("请输入第一个数字: ");
    calc_state.operand2 = getNumberInput("请输入第二个数字: ");
    calc_state.result = calc_state.operand1 + calc_state.operand2;
    calc_state.operation = '+';
    calc_state.has_result = true;
    
    printf("计算结果: %.2f + %.2f = %.2f\n", 
           calc_state.operand1, calc_state.operand2, calc_state.result);
    printf("按回车键继续...\n");
    getchar(); // 清除缓冲区
    getchar(); // 等待用户按键
    
    return nullptr; // 返回到当前菜单
}

static MenuLevel* performSubtraction() {
    printf("\n=== 减法运算 ===\n");
    calc_state.operand1 = getNumberInput("请输入被减数: ");
    calc_state.operand2 = getNumberInput("请输入减数: ");
    calc_state.result = calc_state.operand1 - calc_state.operand2;
    calc_state.operation = '-';
    calc_state.has_result = true;
    
    printf("计算结果: %.2f - %.2f = %.2f\n", 
           calc_state.operand1, calc_state.operand2, calc_state.result);
    printf("按回车键继续...\n");
    getchar();
    getchar();
    
    return nullptr;
}

static MenuLevel* performMultiplication() {
    printf("\n=== 乘法运算 ===\n");
    calc_state.operand1 = getNumberInput("请输入第一个数字: ");
    calc_state.operand2 = getNumberInput("请输入第二个数字: ");
    calc_state.result = calc_state.operand1 * calc_state.operand2;
    calc_state.operation = '*';
    calc_state.has_result = true;
    
    printf("计算结果: %.2f × %.2f = %.2f\n", 
           calc_state.operand1, calc_state.operand2, calc_state.result);
    printf("按回车键继续...\n");
    getchar();
    getchar();
    
    return nullptr;
}

static MenuLevel* performDivision() {
    printf("\n=== 除法运算 ===\n");
    calc_state.operand1 = getNumberInput("请输入被除数: ");
    calc_state.operand2 = getNumberInput("请输入除数: ");
    
    if (calc_state.operand2 == 0.0) {
        printf("错误: 除数不能为零!\n");
        printf("按回车键继续...\n");
        getchar();
        getchar();
        return nullptr;
    }
    
    calc_state.result = calc_state.operand1 / calc_state.operand2;
    calc_state.operation = '/';
    calc_state.has_result = true;
    
    printf("计算结果: %.2f ÷ %.2f = %.2f\n", 
           calc_state.operand1, calc_state.operand2, calc_state.result);
    printf("按回车键继续...\n");
    getchar();
    getchar();
    
    return nullptr;
}

static MenuLevel* showHistory() {
    printf("\n=== 计算历史 ===\n");
    if (calc_state.has_result) {
        printf("上次计算: %.2f %c %.2f = %.2f\n", 
               calc_state.operand1, calc_state.operation, 
               calc_state.operand2, calc_state.result);
    } else {
        printf("暂无计算历史\n");
    }
    printf("按回车键继续...\n");
    getchar();
    getchar();
    
    return nullptr;
}

static MenuLevel* clearCalculator() {
    printf("\n=== 清除计算器 ===\n");
    calc_state.operand1 = 0;
    calc_state.operand2 = 0;
    calc_state.result = 0;
    calc_state.operation = 0;
    calc_state.has_operand1 = false;
    calc_state.has_operand2 = false;
    calc_state.has_result = false;
    
    printf("计算器已清除\n");
    printf("按回车键继续...\n");
    getchar();
    getchar();
    
    return nullptr;
}

// 注册运算菜单项到运算子菜单 - 使用构造函数形式
static MenuItemRegistrar add_operation_registrar(
    &calculator_operations_menu,
    MenuItem{
        "加法 (+)",
        add_icon_bitmap,
        16,
        16,
        []() -> MenuLevel* {
            performAddition();
            return nullptr;
        }
    }
);

static MenuItemRegistrar sub_operation_registrar(
    &calculator_operations_menu,
    MenuItem{
        "减法 (-)",
        sub_icon_bitmap,
        16,
        16,
        []() -> MenuLevel* {
            performSubtraction();
            return nullptr;
        }
    }
);

static MenuItemRegistrar mul_operation_registrar(
    &calculator_operations_menu,
    MenuItem{
        "乘法 (×)",
        mul_icon_bitmap,
        16,
        16,
        []() -> MenuLevel* {
            performMultiplication();
            return nullptr;
        }
    }
);

static MenuItemRegistrar div_operation_registrar(
    &calculator_operations_menu,
    MenuItem{
        "除法 (÷)",
        div_icon_bitmap,
        16,
        16,
        []() -> MenuLevel* {
            performDivision();
            return nullptr;
        }
    }
);

// 注册主菜单项
static MenuItemRegistrar operations_menu_registrar(
    &calculator_main_menu,
    MenuItem{
        "基本运算",
        calculator_icon_bitmap,
        16,
        16,
        []() -> MenuLevel* {
            return &calculator_operations_menu;
        }
    }
);

static MenuItemRegistrar history_registrar(
    &calculator_main_menu,
    MenuItem{
        "计算历史",
        calculator_icon_bitmap,
        16,
        16,
        []() -> MenuLevel* {
            showHistory();
            return nullptr;
        }
    }
);

static MenuItemRegistrar clear_registrar(
    &calculator_main_menu,
    MenuItem{
        "清除",
        calculator_icon_bitmap,
        16,
        16,
        []() -> MenuLevel* {
            clearCalculator();
            return nullptr;
        }
    }
);

// 将计算器主菜单注册为根菜单，标题为"My Application"
static MenuLevelRegistrar calculator_root_registrar(&calculator_main_menu);

// 初始化菜单标题
static void __attribute__((constructor)) init_calculator_menus() {
    strcpy(calculator_main_menu.title, "Calculator");
    strcpy(calculator_operations_menu.title, "计算运算");
}