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
#include "core/NumericFormatter.h"
#include <etl/algorithm.h>

IconViewLayout calculateIconViewLayout(
    int32_t displayWidth, int32_t displayHeight) {
    constexpr int32_t iconWidth = 24;
    constexpr int32_t iconHeight = 24;
    IconViewLayout layout;
    layout.centerX = displayWidth / 2;
    layout.selectorY = (displayHeight - 4) / 2;
    layout.iconY = layout.selectorY - iconHeight / 2;
    const int32_t available = displayWidth - 3 * iconWidth;
    layout.iconSpacing = available > 0 ? available / 4 : 0;
    layout.progressY = displayHeight > 15 ? displayHeight - 15 : 0;
    layout.statusBaseline = displayHeight > 4 ? displayHeight - 4 : 0;
    layout.selectedTitleBaseline = displayHeight > 2 ? displayHeight - 2 : 0;
    const int32_t firstSlot =
        layout.centerX - (3 * iconWidth) / 2 - layout.iconSpacing;
    layout.slotPositionsX = {
        firstSlot,
        firstSlot + iconWidth + layout.iconSpacing,
        firstSlot + 2 * (iconWidth + layout.iconSpacing),
    };
    return layout;
}

/**
 * @brief Construct an IconView instance and initialize slot positions.
 * @param ui Reference to the PixelUI context.
 */
IconView::IconView(PixelUI& ui, const uint8_t * font) : ui_(ui), font_title(font) {
    initializeSlotPositions();
    scrollOffset_ = -ui_.getDisplayWidth();
    animation_selector_coord_x = ui_.getDisplayWidth();
    animation_item_title_Y = ui_.getDisplayHeight() + 6;
}

IconView::~IconView() {
    cancelOwnAnimations();
}

void IconView::cancelOwnAnimations() {
    for (AnimationHandle& handle : animationHandles_) {
        ui_.cancelAnimation(handle);
        handle = INVALID_ANIMATION_HANDLE;
    }
}

bool IconView::animateOwned(
    AnimationSlot slot, int32_t& value, int32_t target,
    uint32_t duration, EasingType easing, PROTECTION protection) {
    AnimationHandle& handle = animationHandles_[static_cast<size_t>(slot)];
    ui_.cancelAnimation(handle);
    handle = INVALID_ANIMATION_HANDLE;
    if (!ui_.animate(value, target, duration, easing, protection, &handle)) {
        value = target;
        return false;
    }
    return true;
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
    cancelOwnAnimations();
    initializeSlotPositions();
    scrollOffset_ = -ui_.getDisplayWidth();

    // Start entry animations for visual transition.
    animateOwned(AnimationSlot::PixelDots, animation_pixel_dots,
                 (ui_.getDisplayWidth() - 2) / 2, 700,
                 EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
    animateOwned(AnimationSlot::SelectorLength, animation_selector_length,
                 selector_length, 700, EasingType::EASE_IN_OUT_CUBIC,
                 PROTECTION::PROTECTED);
    scrollToIndex(currentIndex_);
    ui_.markDirty();  // Trigger initial redraw.
}

/**
 * @brief Called when the view is resumed from a paused state.
 */
void IconView::onResume() {
    animation_scroll_bar = 0;
    scrollOffset_ -= iconWidth_ + 2 * iconSpacing_;
    animateOwned(AnimationSlot::PixelDots, animation_pixel_dots,
                 (ui_.getDisplayWidth() - 2) / 2, 300,
                 EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
    updateProgressBar();
    scrollToIndex(currentIndex_);
    ui_.markDirty();
}

/**
 * @brief Called when the view is paused.
 */
void IconView::onPause() {
    ui_.markFading();
    cancelOwnAnimations();
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
    drawSelector(
        animation_selector_coord_x, layout_.selectorY,
        animation_selector_length);
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
    const int32_t totalItems = static_cast<int32_t>(items_.size());
    if (totalItems == 0) return;

    // Determine target slot position for current index.
    int32_t targetSlot;
    if (newIndex == 0 && totalItems > 1) targetSlot = 0;
    else if (newIndex == totalItems - 1 && totalItems > 1) targetSlot = 2;
    else targetSlot = 1;
    
    // Compute target scroll offset.
    const int32_t targetSelectorX =
        slotPositionsX_[targetSlot] + iconWidth_ / 2;
    const int32_t iconTargetCenterX = targetSelectorX;
    const int32_t iconOriginalCenterX =
        newIndex * (iconWidth_ + iconSpacing_) + iconWidth_ / 2;
    const int32_t targetScrollOffset =
        iconTargetCenterX - iconOriginalCenterX;

    // Animate selector and scroll transitions.
    animateOwned(AnimationSlot::SelectorX, animation_selector_coord_x,
                 targetSelectorX, 550, EasingType::EASE_OUT_CUBIC);
    animateOwned(AnimationSlot::Scroll, scrollOffset_, targetScrollOffset,
                 350, EasingType::EASE_OUT_CUBIC);
    
    // Animate selected item title rise effect.
    if (selectedItemTitleEnabled_) {
        animation_item_title_Y = ui_.getDisplayHeight() + 6;
        animateOwned(AnimationSlot::ItemTitle, animation_item_title_Y,
                     layout_.selectedTitleBaseline, 300,
                     EasingType::EASE_OUT_CUBIC);
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
        const int32_t target = static_cast<int32_t>(
            (static_cast<int64_t>(currentIndex_ + 1) * ui_.getDisplayWidth()) /
            static_cast<int32_t>(items_.size()));
        animateOwned(AnimationSlot::Progress, animation_scroll_bar, target,
                     300, EasingType::EASE_OUT_QUAD);
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
        display.drawPixel(i * 2, layout_.progressY);
    }
    display.drawHLine(0, layout_.progressY, animation_scroll_bar);
}

/**
 * @brief Draw current item index and total count as status text.
 */
void IconView::drawStatusText() {
    if (items_.empty()) return;
    Canvas& display = ui_.getCanvas();
    char statusText[16]{};
    FixedBufferWriter writer(statusText, sizeof(statusText));
    if (!writer.appendInteger(currentIndex_ + 1) || !writer.append("/") ||
        !writer.appendInteger(static_cast<int32_t>(items_.size())) ||
        !writer.finish()) return;
    display.setFont(PIXELUI_FONT_TINY);
    display.drawStr(2, layout_.statusBaseline, statusText);
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
    const int32_t startIndex = getVisibleStartIndex();
    const int32_t endIndex = getVisibleEndIndex();

    for (int32_t i = startIndex;
         i <= endIndex && i < static_cast<int32_t>(items_.size()); ++i) {
        const int32_t iconX = calculateIconX(i);
        drawIcon(items_[i], iconX, iconY_);
    }
}

/**
 * @brief Draw a single icon (bitmap or fallback box).
 * @param item Icon item data containing bitmap pointer and metadata.
 * @param x X coordinate of icon.
 * @param y Y coordinate of icon.
 */
void IconView::drawIcon(const IconItem& item, int32_t x, int32_t y) {
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
    layout_ = calculateIconViewLayout(
        ui_.getDisplayWidth(), ui_.getDisplayHeight());
    centerX_ = layout_.centerX;
    iconY_ = layout_.iconY;
    iconSpacing_ = layout_.iconSpacing;
    slotPositionsX_ = layout_.slotPositionsX;
}

/**
 * @brief Draw a selector frame around the focused icon.
 * @param x X coordinate of selector center.
 * @param y Y coordinate of selector center.
 * @param length Total side length of the selector square.
 */
void IconView::drawSelector(int32_t x, int32_t y, int32_t length) {
    Canvas& display = ui_.getCanvas();
    const int32_t half_length = length / 2;

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
int32_t IconView::calculateIconX(int32_t index) const {
    return (index * (iconWidth_ + iconSpacing_)) + scrollOffset_;
}

/**
 * @brief Determine first visible icon index based on scroll offset.
 * @return Index of the first visible icon.
 */
int32_t IconView::getVisibleStartIndex() const {
    const int32_t leftmostX = -iconWidth_;
    for (int32_t i = 0; i < static_cast<int32_t>(items_.size()); ++i) {
        if (calculateIconX(i) >= leftmostX) return etl::max(0, i - 1);
    }
    return 0;
}

/**
 * @brief Determine last visible icon index based on scroll offset.
 * @return Index of the last visible icon.
 */
int32_t IconView::getVisibleEndIndex() const {
    const int32_t rightmostX = ui_.getDisplayWidth() + iconWidth_;
    for (int32_t i = static_cast<int32_t>(items_.size()) - 1; i >= 0; --i) {
        if (calculateIconX(i) <= rightmostX) {
            return etl::min(static_cast<int32_t>(items_.size()) - 1, i + 1);
        }
    }
    return static_cast<int32_t>(items_.size()) - 1;
}
