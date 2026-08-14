/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ui/IconView/IconView.h"
#include <etl/algorithm.h>
#include <stdio.h>

/**
 * @brief Construct an IconView instance and initialize slot positions.
 * @param ui Reference to the PixelUI context.
 */
IconView::IconView(PixelUI& ui, const uint8_t * font) : ui_(ui), font_title(font) {
    initializeSlotPositions();
}

// -----------------------------------------------------------------------------
// IApplication Interface Implementation
// -----------------------------------------------------------------------------

/**
 * @brief Called when entering the IconView application.
 * @param exitCallback Callback function to trigger when exiting the view.
 */
void IconView::onEnter(ExitCallback exitCallback) {
    IApplication::onEnter(exitCallback);  // Store the exit callback.
    scrollOffset_ = -128;

    // Start entry animations for visual transition.
    ui_.animate(animation_pixel_dots, 63, 700, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
    ui_.animate(animation_selector_length, selector_length, 700, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
    scrollToIndex(currentIndex_);
    ui_.markDirty();  // Trigger initial redraw.
}

/**
 * @brief Called when the view is resumed from a paused state.
 */
void IconView::onResume() {
    animation_scroll_bar = 0;
    scrollOffset_ -= 50;
    ui_.animate(animation_pixel_dots, 63, 300, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
    updateProgressBar();
    scrollToIndex(currentIndex_);
    ui_.markDirty();
}

/**
 * @brief Called when the view is paused.
 */
void IconView::onPause() {
    ui_.markFading();
    ui_.clearAllAnimations();
    animation_selector_length = selector_length;
}

/**
 * @brief Handle input events for navigation and selection.
 * @param event The input event type (LEFT, RIGHT, SELECT, BACK, etc.).
 * @return true if the event was handled, false otherwise.
 */
bool IconView::handleInput(InputEvent event) {
    switch (event) {
        case ICONVIEW_NAVI_LEFT:   navigateLeft(); return true;
        case ICONVIEW_NAVI_RIGHT:  navigateRight(); return true;
        case ICONVIEW_NAVI_SELECT: selectCurrentItem(); return true;
        case ICONVIEW_NAVI_BACK:   requestExit(); return true; // Trigger exit callback.
        default: return false;
    }
}

/**
 * @brief Render all elements of the IconView UI.
 */
void IconView::draw() {
    if (!title_.empty()) drawTitle();
    drawSelector(animation_selector_coord_x, 30, animation_selector_length);
    drawHorizontalIconList();
    if (progressBarEnabled_) drawProgressBar();
    if (statusTextEnabled_) drawStatusText();
    if (selectedItemTitleEnabled_) drawSelectedItemTitle();
}

// -----------------------------------------------------------------------------
// Configuration Methods
// -----------------------------------------------------------------------------

/**
 * @brief Set the list of icons to be displayed.
 * @param items A vector of IconItem structures.
 */
void IconView::setItems(const IconItemList& items) {
    items_ = items;
    currentIndex_ = 0;
}

/**
 * @brief Set callback to be triggered when an icon is selected.
 * @param callback The function to call on selection.
 */
void IconView::setSelectionCallback(SelectionCallback callback) {
    selectionCallback_ = callback;
}

/**
 * @brief Set the title text displayed at the top of the view.
 * @param title Pointer to a C-string title (can be nullptr).
 */
void IconView::setTitle(const char* title) {
    title_ = title ? title : "";
}

void IconView::enableProgressBar(bool enable) { progressBarEnabled_ = enable; }
void IconView::enableStatusText(bool enable) { statusTextEnabled_ = enable; }
void IconView::enableSelectedItemTitle(bool enable) { selectedItemTitleEnabled_ = enable; }

// -----------------------------------------------------------------------------
// Navigation and Interaction
// -----------------------------------------------------------------------------

/**
 * @brief Navigate to the previous icon in the list.
 */
void IconView::navigateLeft() {
    if (items_.empty()) return;
    currentIndex_ = (currentIndex_ - 1 + items_.size()) % items_.size();
    scrollToIndex(currentIndex_);
}

/**
 * @brief Navigate to the next icon in the list.
 */
void IconView::navigateRight() {
    if (items_.empty()) return;
    currentIndex_ = (currentIndex_ + 1) % items_.size();
    scrollToIndex(currentIndex_);
}

/**
 * @brief Select the currently focused icon and trigger the callback.
 */
void IconView::selectCurrentItem() {
    if (selectionCallback_ && !items_.empty()) {
        selectionCallback_(currentIndex_, items_[currentIndex_]);
    }
}

/**
 * @brief Smoothly scroll the view to center the given icon index.
 * @param newIndex The index of the icon to scroll to.
 * @note This method triggers selector and scroll animations.
 */
void IconView::scrollToIndex(int newIndex) {
    int totalItems = items_.size();
    if (totalItems == 0) return;

    // Clear any unprotected animation to avoid overlapping effects.
    ui_.clearUnprotectedAnimations();

    // Determine target slot position for current index.
    int targetSlot;
    if (newIndex == 0 && totalItems > 1) targetSlot = 0;
    else if (newIndex == totalItems - 1 && totalItems > 1) targetSlot = 2;
    else targetSlot = 1;
    
    // Compute target scroll offset.
    float targetSelectorX = slotPositionsX_[targetSlot] + 0.5 * iconWidth_;
    float iconTargetCenterX = slotPositionsX_[targetSlot] + iconWidth_ / 2.0f;
    float iconOriginalCenterX = newIndex * (iconWidth_ + iconSpacing_) + iconWidth_ / 2.0f;
    float targetScrollOffset = iconTargetCenterX - iconOriginalCenterX;

    // Animate selector and scroll transitions.
    ui_.animate(animation_selector_coord_x, targetSelectorX, 550, EasingType::EASE_OUT_CUBIC);
    ui_.animate(scrollOffset_, targetScrollOffset, 350, EasingType::EASE_OUT_CUBIC);
    
    // Animate selected item title rise effect.
    if (selectedItemTitleEnabled_) {
        animation_item_title_Y = 70;
        ui_.animate(animation_item_title_Y, 62, 300, EasingType::EASE_OUT_CUBIC);
    }
    
    currentIndex_ = newIndex;
    updateProgressBar();
    ui_.markDirty();  // Request redraw after animation update.
}

/**
 * @brief Update the progress bar animation according to current selection.
 */
void IconView::updateProgressBar() {
    if (progressBarEnabled_ && !items_.empty()) {
        float progress = (static_cast<float>(currentIndex_ + 1)) / items_.size();
        ui_.animate(animation_scroll_bar, progress * ui_.getDisplayWidth(), 300, EasingType::EASE_OUT_QUAD);
    }
}

// -----------------------------------------------------------------------------
// Drawing Logic
// -----------------------------------------------------------------------------

/**
 * @brief Draw the title text centered at the top of the display.
 */
void IconView::drawTitle() {
    Canvas& display = ui_.getCanvas();
    display.setFont(PIXELUI_FONT_SMALL);
    int titleWidth = display.getStrWidth(title_.c_str());
    display.drawStr((ui_.getDisplayWidth() - titleWidth) / 2, titleY_, title_.c_str());
}

/**
 * @brief Draw the animated progress bar at the bottom.
 */
void IconView::drawProgressBar() {
    Canvas& display = ui_.getCanvas();
    for (int i = 0; i <= static_cast<int>(animation_pixel_dots); i++) {
        display.drawPixel(i * 2, 49);
    }
    display.drawHLine(0, 49, animation_scroll_bar);
}

/**
 * @brief Draw current item index and total count as status text.
 */
void IconView::drawStatusText() {
    if (items_.empty()) return;
    Canvas& display = ui_.getCanvas();
    char statusText[32];
    snprintf(statusText, sizeof(statusText), "%d/%d", currentIndex_ + 1, (int)items_.size());
    display.setFont(PIXELUI_FONT_TINY);
    display.drawStr(2, 60, statusText);
}

/**
 * @brief Draw the title of the currently selected item below the icons.
 */
void IconView::drawSelectedItemTitle() {
    if (items_.empty()) return;
    Canvas& display = ui_.getCanvas();
    const auto& currentItem = items_[currentIndex_];
    display.setFont(font_title);
    int titleWidth = display.getUTF8Width(currentItem.title);
    display.drawUTF8((ui_.getDisplayWidth() - titleWidth) / 2, animation_item_title_Y, currentItem.title);
}

/**
 * @brief Draw visible icons in a horizontal scroll layout.
 */
void IconView::drawHorizontalIconList() {
    if (items_.empty()) {
        // Show a fallback message if no icons exist.
        Canvas& display = ui_.getCanvas();
        display.setFont(PIXELUI_FONT_TINY);
        display.drawStr(centerX_ - 20, iconY_ + 16, "No Items");
        return;
    }
    
    // Determine visible range based on current scroll position.
    int startIndex = getVisibleStartIndex();
    int endIndex = getVisibleEndIndex();

    for (int i = startIndex; i <= endIndex && i < static_cast<int>(items_.size()); ++i) {
        int iconX = calculateIconX(i);
        drawIcon(items_[i], iconX, iconY_);
    }
}

/**
 * @brief Draw a single icon (bitmap or fallback box).
 * @param item Icon item data containing bitmap pointer and metadata.
 * @param x X coordinate of icon.
 * @param y Y coordinate of icon.
 */
void IconView::drawIcon(const IconItem& item, int x, int y) {
    Canvas& display = ui_.getCanvas();
    if (item.bitmap) {
        // Center 24x24 bitmap within icon area.
        int iconX = x + (iconWidth_ - 24) / 2;
        int iconY = y + (iconHeight_ - 24) / 2;
        display.drawXBM(iconX, iconY, 24, 24, item.bitmap);
    } else {
        // Draw placeholder rounded box.
        display.drawRBox(x + 4, y + 4, iconWidth_ - 8, iconHeight_ - 8, 2);
    }
}

/**
 * @brief Precompute X-coordinates for icon slots based on display width.
 */
void IconView::initializeSlotPositions() {
    iconSpacing_ = (ui_.getDisplayWidth() - 3 * iconWidth_) * 0.25f;
    float firstSlotX = centerX_ - 1.5f * iconWidth_ - iconSpacing_;
    slotPositionsX_.clear();
    slotPositionsX_.push_back(firstSlotX);
    slotPositionsX_.push_back(firstSlotX + iconWidth_ + iconSpacing_);
    slotPositionsX_.push_back(firstSlotX + iconWidth_ * 2 + iconSpacing_ * 2);
}

/**
 * @brief Draw a selector frame around the focused icon.
 * @param x X coordinate of selector center.
 * @param y Y coordinate of selector center.
 * @param length Total side length of the selector square.
 */
void IconView::drawSelector(uint32_t x, uint32_t y, uint32_t length) {
    Canvas& display = ui_.getCanvas();
    int half_length = 0.5 * length;

    // Draw corner-style selector lines.
    display.drawLine(x - half_length + 1, y - half_length, x - half_length + 5, y - half_length);
    display.drawLine(x - half_length, y + 1 - half_length, x - half_length, y + 5 - half_length);
    display.drawLine(x - 1 + half_length, y - half_length, x - 5 + half_length, y - half_length);
    display.drawLine(x + half_length, y + 1 - half_length, x + half_length, y + 5 - half_length);
    display.drawLine(x + 1 - half_length, y - 1 + half_length, x + 5 - half_length, y - 1 + half_length);
    display.drawLine(x - half_length, y - 2 + half_length, x - half_length, y - 6 + half_length);
    display.drawLine(x - 1 + half_length, y - 6 + half_length, x - 1 + half_length, y - 2 + half_length);
    display.drawLine(x - 2 + half_length, y - 1 + half_length, x - 6 + half_length, y - 1 + half_length);
}

/**
 * @brief Calculate icon X position based on index and scroll offset.
 * @param index Index of the icon.
 * @return Computed X coordinate.
 */
int IconView::calculateIconX(int index) {
    return (index * (iconWidth_ + iconSpacing_)) + scrollOffset_;
}

/**
 * @brief Determine first visible icon index based on scroll offset.
 * @return Index of the first visible icon.
 */
int IconView::getVisibleStartIndex() {
    int leftmostX = -iconWidth_;
    for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
        if (calculateIconX(i) >= leftmostX) return etl::max(0, i - 1);
    }
    return 0;
}

/**
 * @brief Determine last visible icon index based on scroll offset.
 * @return Index of the last visible icon.
 */
int IconView::getVisibleEndIndex() {
    int rightmostX = 128 + iconWidth_;
    for (int i = static_cast<int>(items_.size()) - 1; i >= 0; --i) {
        if (calculateIconX(i) <= rightmostX) return etl::min(static_cast<int>(items_.size()) - 1, i + 1);
    }
    return static_cast<int>(items_.size()) - 1;
}
