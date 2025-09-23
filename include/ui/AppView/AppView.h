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

    // Implementation of IApplication interface
    void draw() override;
    bool handleInput(InputEvent event) override;
    void onEnter(ExitCallback exitCallback) override;
    void onResume() override;
    void onPause() override;

    // void update(uint32_t currentTime) override;

    // Setters
    void setIconSize(int width, int height);
    void setIconSpacing(int spacing);
    // Getters
    int getCurrentIndex(int index) const { return currentIndex_; }
    bool isAppSelected() const { return appSelected_; }

private:
    PixelUI& ui_;
    AppManager& appManager_;
    ViewManager& m_viewManager;

    void navigateLeft();
    void navigateRight();
    void selectCurrentApp();
    void scrollToIndex(int newIndex);

    int currentIndex_ = 0;      // Index of the currently selected app
    bool appSelected_ = false;  // Whether an app is selected

    int32_t scrollOffset_ = -128;        // Current scroll offset
    int32_t targetOffset_ = 0;           // Target scroll offset
    int32_t appTitle_Y = 70;             // Y-coordinate of the app title
    int32_t animation_selector_coord_x = 128;  // Animation selector X-coordinate
    int32_t animation_selector_length = 10;    // Animation selector length

    int iconWidth_ = 24;        // Icon width
    int iconHeight_ = 24;       // Icon height
    int iconSpacing_ = 14;      // Spacing between icons
    int centerX_ = 64;          // Screen center X-coordinate (128/2)
    int iconY_ = 18;            // Y-coordinate for icons
    
    int32_t animation_pixel_dots = 0;    // Animation state for pixel dots
    int32_t animation_scroll_bar = 0;    // Animation state for scroll bar

    int32_t selector_length = 30;        // Selector length

    void drawHorizontalAppList();
    void drawAppIcon(const AppItem& app, int x, int y, bool inCenter);
    void drawSelector(uint32_t x, uint32_t y, uint32_t length);
    // void drawEntranceAnimation();

    void updateProgressBar();

    int calculateIconX(int index);
    int getVisibleStartIndex();
    int getVisibleEndIndex();

    std::vector<float> slotPositionsX_;  // Precomputed icon slot positions
};
