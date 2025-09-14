/*
 * Copyright (C) 2025 Lawrence Link
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "PixelUI.h"
#include "core/app/app_system.h"
#include "core/app/IApplication.h"
#include "core/ViewManager/ViewManager.h"

class AppView : public IApplication {
public:
    AppView(PixelUI& ui, ViewManager& viewManager);
    ~AppView() = default;

    // 实现 IApplication 接口
    void draw() override;
    bool handleInput(InputEvent event) override;
    void onEnter(ExitCallback exitCallback) override;
    void onResume() override ;
    void onPause() override;

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

    int32_t scrollOffset_ = -128;        // 当前滚动偏移量
    int32_t targetOffset_ = 0;        // 目标滚动偏移量
    int32_t appTitle_Y = 70;
    int32_t animation_selector_coord_x = 128;
    int32_t animation_selector_length = 10;

    int iconWidth_ = 24;               // 图标宽度
    int iconHeight_ = 24;              // 图标高度
    int iconSpacing_ = 14;             // 图标间距
    int centerX_ = 64;                 // 屏幕中心X坐标 (128/2)
    int iconY_ = 18;                   // 图标Y坐标
    
    int32_t animation_pixel_dots = 0;
    int32_t animation_scroll_bar= 0;

    int32_t selector_length = 30;

    void drawHorizontalAppList();
    void drawAppIcon(const AppItem& app, int x, int y, bool inCenter);
    void drawSelector(uint32_t x, uint32_t y, uint32_t length);
    // void drawEntranceAnimation();

    void updateProgressBar();

    int calculateIconX(int index);
    // void scrollToIndex(int index);
    int getVisibleStartIndex();
    int getVisibleEndIndex();

    std::vector<float> slotPositionsX_;
};