#pragma once

#include "PixelUI/pixelui.h"
#include "PixelUI/core/app/app_system.h"
#include "PixelUI/core/app/IApplication.h"
#include "PixelUI/core/ViewManager/ViewManager.h"

class AppView : public IApplication {
public:
    AppView(PixelUI& ui, ViewManager& viewManager);
    ~AppView() = default;

    // 实现 IApplication 接口
    void draw() override;
    bool handleInput(InputEvent event) override;
    void onEnter(ExitCallback exitCallback) override;
    void onResume() override ;

    // void update(uint32_t currentTime) override;

    // setters
    void setIconSize(int width, int height);
    void setIconSpacing(int spacing);
    // getters
    int getCurrentIndex(int index) const { return currentIndex_ ; }
    bool isAppSelected() const { return appSelected_; }

private:
    PixelUI& ui_;
    AppManager& appManager_;
    ViewManager& m_viewManager;

    void navigateLeft();
    void navigateRight();
    void selectCurrentApp();
    void scrollToIndex(int newIndex);

    int currentIndex_ = 0; // 当前选中的应用索引
    bool appSelected_ = false; // 是否选中了应用

    float scrollOffset_ = -100.0f;        // 当前滚动偏移量
    float targetOffset_ = 0.0f;        // 目标滚动偏移量
    float appTitle_Y = 70.0f;
    
    int iconWidth_ = 24;               // 图标宽度
    int iconHeight_ = 24;              // 图标高度
    int iconSpacing_ = 14;             // 图标间距
    int centerX_ = 64;                 // 屏幕中心X坐标 (128/2)
    int iconY_ = 14;                   // 图标Y坐标
    
    float animation_pixel_dots = 0;
    float animation_scroll_bar= 0;

    float selector_length = 28;

    void drawHorizontalAppList();
    void drawAppIcon(const AppItem& app, int x, int y, bool inCenter);
    void drawSelector(uint32_t x, uint32_t y, uint32_t length);

    void updateProgressBar();

    int calculateIconX(int index);
    // void scrollToIndex(int index);
    int getVisibleStartIndex();
    int getVisibleEndIndex();

    std::vector<float> slotPositionsX_;
};

void startAppView(PixelUI& ui);