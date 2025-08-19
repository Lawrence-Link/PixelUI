#pragma once

#include "PixelUI/pixelui.h"
#include "PixelUI/core/app/app_system.h"

class AppView : public IDrawable {
public:
    AppView(PixelUI& ui);
    ~AppView() = default;

    void draw() override;

    void update(uint32_t currentTime) override;

    void navigateLeft();
    void navigateRight();
    void selectCurrentApp();
    void goBack();
    
    void setIconSize(int width, int height);
    void setIconSpacing(int spacing);

    int getCurrentIndex(int index) const { return currentIndex_ ; }
    bool isAppSelected() const { return appSelected_; }

private:
    PixelUI& ui_;
    AppManager& appManager_;
    int currentIndex_ = 0; // 当前选中的应用索引
    bool appSelected_ = false; // 是否选中了应用

    float scrollOffset_ = 0.0f;        // 当前滚动偏移量
    float targetOffset_ = 0.0f;        // 目标滚动偏移量
    int iconWidth_ = 24;               // 图标宽度
    int iconHeight_ = 24;              // 图标高度
    int iconSpacing_ = 14;             // 图标间距
    int centerX_ = 64;                 // 屏幕中心X坐标 (128/2)
    int iconY_ = 14;                   // 图标Y坐标

// 选择框参数
    int selectorWidth_ = 40;           // 选择框宽度
    int selectorHeight_ = 40;          // 选择框高度
    int selectorX_;                    // 选择框X坐标（居中）
    int selectorY_;                    // 选择框Y坐标

    void drawHorizontalAppList();
    void drawAppIcon(const AppItem& app, int x, int y, bool inCenter);
    void drawSelector(uint32_t x, uint32_t y, uint32_t length);
    int calculateIconX(int index);
    void scrollToIndex(int index);
    int getVisibleStartIndex();
    int getVisibleEndIndex();
};

void startAppView(PixelUI& ui);