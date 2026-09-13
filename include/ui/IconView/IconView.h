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

#pragma once

#include "PixelUI.h"
#include "core/app/IApplication.h"
#include "widgets/label/label.h"
#include <etl/inplace_function.h>
#include <etl/string.h>
#include <etl/vector.h>
#include <etl/array.h>

// A generic interface for an icon-based item.
struct IconItem {
    // All pointers are non-owning and must outlive the IconView storing this item.
    const char* title;
    const uint8_t* bitmap;
    void* userData; // Used to store type-specific data.
    
    IconItem(const char* t, const uint8_t* b, void* data = nullptr) 
        : title(t), bitmap(b), userData(data) {}
};

template <size_t Capacity>
using IconItemList = etl::vector<IconItem, Capacity>;
// The callback object is owned; references captured by it are non-owning.
using SelectionCallback = etl::inplace_function<void(int index, const IconItem& item), CALLBACK_STORAGE_SIZE>;

struct IconViewLayout {
    int32_t centerX = 0;
    int32_t selectorY = 0;
    int32_t iconY = 0;
    int32_t iconSpacing = 0;
    int32_t progressY = 0;
    int32_t statusBaseline = 0;
    int32_t selectedTitleBaseline = 0;
    int32_t selectedTitleX = 0;
    int32_t selectedTitleWidth = 0;
    etl::array<int32_t, 3> slotPositionsX{};
};

// All division uses integer truncation toward zero. Inputs are non-negative
// display dimensions, so this is an explicit floor rule; odd leftovers stay on
// the right/bottom edge.
IconViewLayout calculateIconViewLayout(int32_t displayWidth, int32_t displayHeight);

namespace icon_view_detail {

template <size_t Capacity>
class IconItemStorage {
    static_assert(Capacity > 0, "IconView capacity must be positive");

protected:
    IconItemList<Capacity> items_;
};

class IconViewBase : public IApplication {
protected:
    IconViewBase(PixelUI& ui, etl::ivector<IconItem>& items,
                 const uint8_t* font);

public:
    ~IconViewBase() override;

    // --- IApplication Interface Implementation ---
    void draw() override;
    bool handleInput(InputEvent event) override;
    void onEnter(ExitCallback exitCallback) override;
    void onResume() override;
    void onPause() override;

    // Returns false without changing the current items when capacity is exceeded.
    // External animation registration starts in onEnter().
    bool setItems(const etl::ivector<IconItem>& items);
    void setSelectionCallback(SelectionCallback callback);
    void setTitle(const char* title);
    
    // Control additional UI elements.
    void enableProgressBar(bool enable);
    void enableStatusText(bool enable);
    void enableSelectedItemTitle(bool enable);

private:
    static constexpr int32_t TITLE_Y = 10;
    static constexpr int32_t ICON_WIDTH = 24;
    static constexpr int32_t ICON_HEIGHT = 24;
    static constexpr int32_t SELECTOR_LENGTH = 32;

    PixelUI& ui_;
    etl::ivector<IconItem>& items_;
    IconViewLayout layout_{};
    Label selectedItemTitle_;
    SelectionCallback selectionCallback_;
    
    // Title-related members.
    etl::string<MAX_TEXT_LENGTH> title_;
    
    // State.
    int32_t currentIndex_ = 0;
    
    // Toggles for UI elements.
    bool progressBarEnabled_ = false;
    bool statusTextEnabled_ = false;
    bool selectedItemTitleEnabled_ = false;
    bool active_ = false;

    // Animation variables.
    int32_t scrollOffset_ = 0;
    int32_t animation_selector_coord_x = 0;
    int32_t animation_selector_length = 10;
    int32_t animation_item_title_Y = 0;
    int32_t animation_pixel_dots = 0;
    int32_t animation_scroll_bar = 0;

    enum class AnimationSlot : uint8_t {
        PixelDots,
        SelectorLength,
        SelectorX,
        Scroll,
        ItemTitle,
        Progress,
        Count,
    };
    etl::array<AnimationHandle, static_cast<size_t>(AnimationSlot::Count)>
        animationHandles_{};
    
    // --- Private Methods ---
    void initializeSlotPositions();
    void cancelOwnAnimations();
    bool animateOwned(AnimationSlot slot, int32_t& value, int32_t target,
                      uint32_t duration, EasingType easing,
                      PROTECTION protection = PROTECTION::NOT_PROTECTED);
    void navigateLeft();
    void navigateRight();
    void selectCurrentItem();
    void scrollToIndex(int newIndex);
    void updateProgressBar();
    void updateSelectedItemTitle();

    // Drawing logic.
    void drawTitle();
    void drawSelector(int32_t x, int32_t y, int32_t length);
    void drawHorizontalIconList();
    void drawIcon(const IconItem& item, int32_t x, int32_t y);
    void drawProgressBar();
    void drawStatusText();
    void drawSelectedItemTitle();
    
    int32_t calculateIconX(int32_t index) const;
    int32_t getVisibleStartIndex() const;
    int32_t getVisibleEndIndex() const;
};

} // namespace icon_view_detail

// Capacity is selected per view while storage remains fixed and allocation-free.
template <size_t Capacity>
class IconView : private icon_view_detail::IconItemStorage<Capacity>,
                 public icon_view_detail::IconViewBase {
    using Storage = icon_view_detail::IconItemStorage<Capacity>;

public:
    explicit IconView(PixelUI& ui, const uint8_t* font = PIXELUI_FONT_TEXT)
        : Storage(), icon_view_detail::IconViewBase(ui, Storage::items_, font) {}

    IconView(const IconView&) = delete;
    IconView& operator=(const IconView&) = delete;
    IconView(IconView&&) = delete;
    IconView& operator=(IconView&&) = delete;
};
