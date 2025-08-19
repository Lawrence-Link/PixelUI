#include "PixelUI/core/ui/AppView/Appview.h"
#include <iostream>
#include <algorithm>
#include <cstring>

AppView::AppView(PixelUI& ui) : ui_(ui), appManager_(AppManager::getInstance()) {
    currentIndex_ = 0;  // 初始化当前索引为0
    appSelected_ = false; // 初始化应用未被选中
    iconSpacing_ = (ui.getU8G2().getWidth() - 3* iconWidth_ )* 0.25;
}

void AppView::draw() {
    U8G2Wrapper& display = ui_.getU8G2();
    
    static bool run_once = false;

    static float animation_pixel_dots = 0;
    static float animation_scroll_bar= 0;
    
    static float animation_selector_coord_x = 128;
    static float animation_selector_length = 10;

    if (!run_once) {
        run_once = true;
        // 初始化应用列表 loading scene
        ui_.animate(animation_pixel_dots, 63, 300, EasingType::EASE_IN_OUT_CUBIC);
        ui_.animate(animation_scroll_bar, 127, 1000, EasingType::EASE_IN_OUT_CUBIC);
        ui_.animate(animation_selector_coord_x, 50,  500, EasingType::EASE_IN_OUT_CUBIC);
        ui_.animate(animation_selector_length, 28,  500, EasingType::EASE_IN_OUT_CUBIC);
    }

    // 设置字体
    display.setFont(u8g2_font_tom_thumb_4x6_mf);
    
    // 绘制标题
    display.drawStr(2, 10, "Apps");
    
    // 绘制选择框
    drawSelector(animation_selector_coord_x, 12, animation_selector_length);
    
    // 绘制水平应用列表
    drawHorizontalAppList();
    
    // draw dotline to the progress bar:

    for (int i = 0; i <= static_cast<int> (animation_pixel_dots); i++)
        display.drawPixel(i * 2, 50);
    display.drawBox(0, 49, animation_scroll_bar, 3);

    // 绘制状态信息
    const auto& apps = appManager_.getAppVector();
    if (!apps.empty()) {
        char statusText[16];
        snprintf(statusText, sizeof(statusText), "%d/%d", currentIndex_ + 1, (int)apps.size());
        display.drawStr(2, 60, statusText);
    }
}

void AppView::drawSelector(uint32_t x, uint32_t y, uint32_t length) {
    U8G2Wrapper& display = ui_.getU8G2();
    
    // Top left corner of the selector
    display.drawLine(x+1, y, x+5, y);
    display.drawLine(x, y+1, x, y+5);
    // Top right corner of the selector
    display.drawLine(x+length-1, y, x+length-5, y);
    display.drawLine(x+length, y+1, x+length, y+5);
    // Bottom left corner of the selector
    display.drawLine(x+1, y+length-1, x+5, y+length-1);
    display.drawLine(x, y+length-2, x, y+length-6);
    // Bottom right corner of the selector
    display.drawLine(x+length-1, y+length-6, x+length-1, y+length-2);
    display.drawLine(x+length-2, y+length-1, x+length-6, y+length-1);
}

int AppView::calculateIconX(int index) {
    // 计算图标的X坐标，基于滚动偏移量
    int baseX = centerX_ + (index - currentIndex_) * (iconWidth_ + iconSpacing_) - iconWidth_ / 2;
    return baseX + static_cast<int>(scrollOffset_);
}

void AppView::drawHorizontalAppList() {
    const auto& apps = appManager_.getAppVector();
    if (apps.empty()) {
        U8G2Wrapper& display = ui_.getU8G2();
        display.drawStr(centerX_ - 20, iconY_ + 16, "No Apps");
        return;
    }
    
    // 计算可见范围（包括画布外的图标）
    int startIndex = getVisibleStartIndex();
    int endIndex = getVisibleEndIndex();

    std::cout << "[DEBUG] Drawing icons from " << startIndex << " to " << endIndex << std::endl;

    // 绘制所有可能可见的图标（包括部分可见的）
    for (int i = startIndex; i <= endIndex && i < static_cast<int>(apps.size()); ++i) {
        int iconX = calculateIconX(i);
        // 判断是否在中心区域
        bool inCenter = (i == currentIndex_);
        std::cout << "[DEBUG] Icon " << i << " at X=" << iconX << (inCenter ? " [CENTER]" : "") << std::endl;
        drawAppIcon(apps[i], iconX, iconY_, inCenter);
    }
}

void AppView::drawAppIcon(const AppItem& app, int x, int y, bool inCenter) {
    U8G2Wrapper& display = ui_.getU8G2();
    
    // 绘制应用图标
    if (app.bitmap) {
        // 居中绘制图标
        int iconX = x + (iconWidth_ - app.w) / 2;
        int iconY = y + (iconHeight_ - app.h) / 2;
        display.drawXBM(iconX, iconY, app.w, app.h, app.bitmap);
    } else {
        // 如果没有图标，绘制一个简单的矩形
        display.drawRBox(x + 4, y + 4, iconWidth_ - 8, iconHeight_ - 8, 2);
    }
    
    // 绘制应用标题（在图标下方）
    display.setFont(u8g2_font_tom_thumb_4x6_mf);
    
    // 计算文字居中位置
    int textWidth = strlen(app.title) * 4; // 大概估算
    int textX = x + (iconWidth_ - textWidth) / 2;
    display.drawStr(textX, y + iconHeight_ + 8, app.title);
    
    // 如果在中心，可以添加特殊效果
    if (inCenter) {
        // 可以添加高亮效果，但选择框已经在drawCenterSelector中绘制了
        std::cout << "[DEBUG] Drew center app: " << app.title << " at (" << x << "," << y << ")" << std::endl;
    }
}

int AppView::getVisibleStartIndex() {
    // 计算最左边可能可见的图标索引（包括部分可见）
    const auto& apps = appManager_.getAppVector();
    int leftmostX = -iconWidth_; // 允许完全在左边画布外

    for (int i = 0; i < static_cast<int>(apps.size()); ++i) {
        if (calculateIconX(i) >= leftmostX) {
            return std::max(0, i - 1); // 多绘制一个以防万一
        }
    }
    return 0;
}

int AppView::getVisibleEndIndex() {
    // 计算最右边可能可见的图标索引（包括部分可见）
    const auto& apps = appManager_.getAppVector();
    int rightmostX = 128 + iconWidth_; // 允许完全在右边画布外
    
    for (int i = static_cast<int>(apps.size()) - 1; i >= 0; --i) {
        if (calculateIconX(i) <= rightmostX) {
            return std::min(static_cast<int>(apps.size()) - 1, i + 1); // 多绘制一个以防万一
        }
    }
    return static_cast<int>(apps.size()) - 1;
}

void AppView::scrollToIndex(int index) {
    // 计算目标偏移量，使指定图标居中
    targetOffset_ = 0.0f; // 当前图标应该在中心，所以偏移量为0
    
    // 启动平滑滚动动画
    ui_.animate(scrollOffset_, targetOffset_, 300, EasingType::EASE_OUT_QUAD);
    
    ui_.markDirty(); // 标记需要重绘
}

void AppView::selectCurrentApp() {
    const auto& apps = appManager_.getAppVector();
    if (currentIndex_ >= 0 && currentIndex_ < static_cast<int>(apps.size())) {
        std::cout << "[DEBUG] Launching app: " << apps[currentIndex_].title << std::endl;
        if (apps[currentIndex_].action) {
            apps[currentIndex_].action();
        }
        appSelected_ = true;
    }
}

void AppView::setIconSpacing(int spacing) {
    iconSpacing_ = spacing;
}

void AppView::update(uint32_t currentTime) {

}

void AppView::goBack() {
    std::cout << "[DEBUG] AppView go back requested" << std::endl;
}

void startAppView(PixelUI& ui) {
    auto appView = new AppView(ui);
    ui.setDrawable(appView);
    std::cout << "[DEBUG] Horizontal scrolling AppView started successfully" << std::endl;
}