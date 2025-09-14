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

#include "ui/AppView/AppView.h"
#include <algorithm>
#include <cstring>

AppView::AppView(PixelUI& ui, ViewManager& viewManager) : ui_(ui), appManager_(AppManager::getInstance()), m_viewManager(viewManager) {
    iconSpacing_ = (ui.getU8G2().getWidth() - 3 * iconWidth_ )* 0.25;

    float totalListWidth = 3 * iconWidth_ + 2 * iconSpacing_;
    float firstSlotX = centerX_ - 1.5f * iconWidth_ - iconSpacing_;

    slotPositionsX_.push_back(firstSlotX);
    slotPositionsX_.push_back(firstSlotX + iconWidth_ + iconSpacing_);
    slotPositionsX_.push_back(firstSlotX + iconWidth_ * 2 + iconSpacing_ * 2);
    scrollOffset_ = slotPositionsX_[0] - /*calculateIconX(0)*/ + scrollOffset_;
    scrollToIndex(0);
}

/*
    @brief Update the progress bar based on the current app index
*/
void AppView::updateProgressBar() {
    const auto& apps = appManager_.getAppVector();
    ui_.animate(animation_scroll_bar, (static_cast<float>((currentIndex_ + 1)) / static_cast<float>(apps.size())) * ui_.getU8G2().getWidth(), 300, EasingType::EASE_OUT_QUAD);
}

void AppView::onEnter(ExitCallback exitCallback) {
    IApplication::onEnter(exitCallback);
    // Start entry animations
    ui_.animate(animation_pixel_dots, 63, 300, EasingType::EASE_IN_OUT_CUBIC);
    ui_.animate(animation_selector_length, selector_length, 700, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
    ui_.markDirty();
}

void AppView::onResume() {
    animation_scroll_bar = 0;
    scrollOffset_ -= 50;

    ui_.animate(animation_pixel_dots, 63, 300, EasingType::EASE_IN_OUT_CUBIC);
    updateProgressBar();
    scrollToIndex(currentIndex_);
    ui_.animate(animation_selector_length, selector_length, 700, EasingType::EASE_IN_OUT_CUBIC);
    ui_.markDirty();
}

void AppView::onPause() {
    ui_.markFading();
}

bool AppView::handleInput(InputEvent event) {
    switch (event) {
        case InputEvent::LEFT: navigateLeft(); return true;
        case InputEvent::RIGHT: navigateRight(); return true;
        case InputEvent::SELECT: selectCurrentApp(); return true;
        case InputEvent::BACK: return true;
        default: return false;
    }
}

void AppView::navigateLeft() {
    const auto& apps = appManager_.getAppVector();
    currentIndex_--;
    if (currentIndex_ < 0) {
        currentIndex_ = apps.size() - 1;
    }
    scrollToIndex(currentIndex_);
}

void AppView::navigateRight() {
    const auto& apps = appManager_.getAppVector();
    currentIndex_++;
    if (currentIndex_ >= static_cast<int>(apps.size())) {
        currentIndex_ = 0;
    }
    scrollToIndex(currentIndex_);
}

void AppView::draw() {
    U8G2& display = ui_.getU8G2();
    
    // Set font and draw title
    display.setFont(u8g2_font_tom_thumb_4x6_mf);
    display.drawStr((display.getWidth() - display.getStrWidth("< Apps >")) / 2, 10, "< Apps >");
    
    // Draw selector and app list
    drawSelector(animation_selector_coord_x, 30, animation_selector_length);
    drawHorizontalAppList();
    
    // Draw progress bar
    for (int i = 0; i <= static_cast<int>(animation_pixel_dots); i++) {
        display.drawPixel(i * 2, 50);
    }
    display.drawHLine(0, 50, animation_scroll_bar);

    // Draw status info
    const auto& apps = appManager_.getAppVector();
    if (!apps.empty()) {
        char statusText[16];
        snprintf(statusText, sizeof(statusText), "%d/%d", currentIndex_ + 1, (int)apps.size());
        display.drawStr(2, 60, statusText);
    }
}

void AppView::selectCurrentApp() {
    const auto& apps = appManager_.getAppVector();
    if (apps.empty() || currentIndex_ < 0 || currentIndex_ >= static_cast<int>(apps.size())) {
        return;
    }

    const auto& selectedAppItem = apps[currentIndex_];
    
    if (selectedAppItem.createApp) {
        auto appInstance = selectedAppItem.createApp(ui_);

        if (appInstance) {
            // Push the new app instance to the view manager
            m_viewManager.push(appInstance);
        }
    }
}

// Draw the selector box at the specified coordinates
void AppView::drawSelector(uint32_t x, uint32_t y, uint32_t length) {
    U8G2& display = ui_.getU8G2();

    int half_length = 0.5 * length;

    // Top left corner
    display.drawLine(x - half_length + 1, y - half_length, x - half_length + 5, y - half_length);
    display.drawLine(x - half_length, y + 1 - half_length, x - half_length, y + 5 - half_length);
    // Top right corner
    display.drawLine(x - 1 + half_length, y - half_length, x - 5 + half_length, y - half_length);
    display.drawLine(x + half_length, y + 1 - half_length, x + half_length, y + 5 - half_length);
    // Bottom left corner
    display.drawLine(x + 1 - half_length, y - 1 + half_length, x + 5 - half_length, y - 1 + half_length);
    display.drawLine(x - half_length, y - 2 + half_length, x - half_length, y - 6 + half_length);
    // Bottom right corner
    display.drawLine(x - 1 + half_length, y - 6 + half_length, x - 1 + half_length, y - 2 + half_length);
    display.drawLine(x - 2 + half_length, y - 1 + half_length, x - 6 + half_length, y - 1 + half_length);
}

int AppView::calculateIconX(int index) {
    return (index * (iconWidth_ + iconSpacing_)) + scrollOffset_;
}

void AppView::drawHorizontalAppList() {
    const auto& apps = appManager_.getAppVector();
    if (apps.empty()) {
        U8G2& display = ui_.getU8G2();
        display.drawStr(centerX_ - 20, iconY_ + 16, "No Apps");
        return;
    }
    
    int startIndex = getVisibleStartIndex();
    int endIndex = getVisibleEndIndex();

    // Draw all visible icons
    for (int i = startIndex; i <= endIndex && i < static_cast<int>(apps.size()); ++i) {
        int iconX = calculateIconX(i);
        bool inCenter = (i == currentIndex_);
        drawAppIcon(apps[i], iconX, iconY_, inCenter);
    }
}

void AppView::drawAppIcon(const AppItem& app, int x, int y, bool inCenter) {
    U8G2& display = ui_.getU8G2();
    
    // Draw the app icon
    if (app.bitmap) {
        int iconX = x + (iconWidth_ - 24) / 2;
        int iconY = y + (iconHeight_ - 24) / 2;
        display.drawXBM(iconX, iconY, 24, 24, app.bitmap);
    } else {
        // Draw a placeholder rectangle if no icon is provided
        display.drawRBox(x + 4, y + 4, iconWidth_ - 8, iconHeight_ - 8, 2);
    }
    
    // Draw the app title below the icon
    display.setFont(u8g2_font_tom_thumb_4x6_mf);
    
    if (inCenter) {
        display.drawStr((display.getWidth() - display.getStrWidth(app.title)) / 2, appTitle_Y, app.title);
    }
}

int AppView::getVisibleStartIndex() {
    const auto& apps = appManager_.getAppVector();
    int leftmostX = -iconWidth_; // Account for icons partially off-screen
    for (int i = 0; i < static_cast<int>(apps.size()); ++i) {
        if (calculateIconX(i) >= leftmostX) {
            return std::max(0, i - 1);
        }
    }
    return 0;
}

int AppView::getVisibleEndIndex() {
    const auto& apps = appManager_.getAppVector();
    int rightmostX = 128 + iconWidth_; // Account for icons partially off-screen
    for (int i = static_cast<int>(apps.size()) - 1; i >= 0; --i) {
        if (calculateIconX(i) <= rightmostX) {
            return std::min(static_cast<int>(apps.size()) - 1, i + 1);
        }
    }
    return static_cast<int>(apps.size()) - 1;
}

void AppView::scrollToIndex(int newIndex) {
    const auto& apps = appManager_.getAppVector();
    int totalApps = apps.size();
    if (totalApps == 0) return;

    ui_.getAnimationManPtr()->clearUnprotected();

    int targetSlot;
    if (newIndex == 0) {
        targetSlot = 0;
    } else if (newIndex == totalApps - 1 && totalApps > 1) {
        targetSlot = 2;
    } else {
        targetSlot = 1;
    }
    if (totalApps == 1) {
        targetSlot = 1;
    }

    float targetSelectorX = slotPositionsX_[targetSlot] + 0.5 * iconWidth_;
    float iconTargetCenterX = slotPositionsX_[targetSlot] + iconWidth_ / 2.0f;
    float iconOriginalCenterX = newIndex * (iconWidth_ + iconSpacing_) + iconWidth_ / 2.0f;
    float targetScrollOffset = iconTargetCenterX - iconOriginalCenterX;

    ui_.animate(animation_selector_coord_x, targetSelectorX, 550, EasingType::EASE_OUT_CUBIC);
    ui_.animate(scrollOffset_, targetScrollOffset, 350, EasingType::EASE_OUT_CUBIC);
    
    updateProgressBar();
    
    // Title popup effect
    appTitle_Y = 70;
    ui_.animate(appTitle_Y, 60, 300, EasingType::EASE_OUT_CUBIC);

    currentIndex_ = newIndex;
    ui_.markDirty(); 
}

void AppView::setIconSpacing(int spacing) {
    iconSpacing_ = spacing;
}