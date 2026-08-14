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
#include <etl/algorithm.h>
#include "etl/vector.h"
#include "etl/delegate.h"
#include "core/animation/animation.h"
#include "core/app/IApplication.h"
#include "etl/map.h"
#include <stdint.h>

// Struct to hold extra data for a list item, like values for switches or sliders.
struct ListItemExtra{
    char* text = nullptr;
    bool* switchValue = nullptr; // Pointer to a boolean value for a switch.
    int32_t* intValue = nullptr;     // Pointer to an integer value for a slider or counter.
    float* float_dot1f_Value = nullptr;
};

// Represents a single item in a list view.
struct ListItem{
    mutable char title[MAX_LISTITEM_NAME_NUM]; // The display title of the item. 'mutable' allows it to be changed even if the struct is 'const'.
    ListItem * nextList = nullptr;                       // Pointer to a sub-menu (another list).
    int32_t nextListLength = 0;                     // The number of items in the sub-menu. (signed to avoid mixed-signedness)
    VoidCallback pFunc = nullptr;               // A function to execute when the item is selected.
    ListItemExtra extra = {nullptr,nullptr};                       // Extra data for dynamic UI elements.
    bool use_fade = false; // Whether render fade animation when navigate to new app.
};

// The main class for handling a list-based user interface.
class ListView : public IApplication {
public:
    // Constructor to initialize the list view with a UI handler and a list of items.
    // NOTE: length is an int (signed) to match internal usage of indices and avoid
    // signed/unsigned comparison warnings.
    ListView(PixelUI& ui, ListItem *itemList, int length)
        : IApplication(true), m_ui(ui), m_itemList(itemList), m_itemLength(length - 1) {}
    ~ListView() = default;

    // --- Application Lifecycle and Input Handlers ---
    void draw() override;
    bool handleInput(InputEvent event) override;
    void onEnter(ExitCallback exitCallback) override;
    void onResume() override ;
    void onPause() override;
    void onExit() override;

    virtual void onLoad() = 0;
    virtual void onSave() = 0;

    // --- Public Utility Methods ---
    void resizeLength(int itemLength) { m_itemLength = itemLength; }
    PixelUI& getUI() { return m_ui; }
    
    PixelUI& m_ui; // Reference to the main UI class.
private:
    ListItem* m_itemList;
    int32_t m_itemLength; // signed index length (last index). Avoid mixing signed/unsigned.
    
    struct SwitchAnimState {
    int32_t boxX = 0;
    bool isAnimating = false;
};
    etl::map<int, SwitchAnimState, MAX_LISTVIEW_SLOT_NUM> switchAnimStates_;

    // --- Layout and Spacing Variables ---
    uint8_t spacing_ = 7;
    uint8_t topMargin_ = 3;
    uint8_t FontHeight = 0;
    
    // History stack to support nested menus (for back navigation).
    etl::vector<etl::pair<etl::pair<ListItem*, int32_t>, int32_t>, MAX_LISTVIEW_DEPTH> m_history_stack;

    // --- Cursor Variables ---
    int32_t CursorY = -6;
    int32_t CursorX = 1;
    int32_t CursorWidth = 0;
    
    // --- Scroll Variables ---
    int32_t topVisibleIndex_ = 0;           // Index of the first item visible on screen.
    int32_t visibleItemCount_ = LISTVIEW_ITEMS_PER_PAGE; // Number of items that can be displayed at once.
    
    // --- Load Animation Variables ---
    int32_t itemLoadAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1]; // Tracks animation progress for each item.
    bool isInitialLoad_ = true;
    int32_t animation_pixel_dots = 0;
    int32_t animation_scroll_bar = 0;
    
    // --- Transition Animation Variables ---
    bool isTransitioning_ = false;
    int32_t transitionProgress_ = 0;
    int32_t selectedItemForTransition_ = -1;
    int32_t itemExitAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1]; // Animations for items leaving the screen.
    int32_t itemEnterAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1]; // Animations for items entering the screen.
    int32_t selectedItemY_ = 0;
    ListItem* oldItemList_ = nullptr;
    int32_t oldItemLength_ = 0;
    int32_t oldTopVisibleIndex_ = 0;
    // --- Progress Bar Variables ---
    int32_t progress_bar_top = 0;
    int32_t progress_bar_bottom = 0;

    // --- Navigation and Drawing Methods ---
    void navigateLeft();
    void navigateRight();
    void navigateUp();
    void navigateDown();

    void drawCursor();
    void scrollToTarget();
    void updateScrollPosition();
    void startLoadAnimation();
    void startTransitionAnimation(int selectedItemIndex);
    int getVisibleItemIndex(int screenIndex);
    bool shouldScroll(int newCursor);
    int32_t calculateItemY(int itemIndex);
    
    void selectCurrent();
    void returnToPreviousContext();

    void clearNonInitialAnimations();
    
    int32_t currentCursor = 0; // The index of the currently selected item. (signed)
};
