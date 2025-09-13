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

#include "PixelUI/core/ui/AppView/AppView.h"
#include "PixelUI/pixelui.h"
#include <algorithm>
#include <cstring>
#include "etl/vector.h"
#include "etl/delegate.h"
#include "PixelUI/core/animation/animation.h" // 确保包含 animation.h 以访问定点数宏

// 定义一个宏，用于将浮点数常量转换为定点数


struct ListItemExtra{
    bool* switchValue = nullptr;
    int* intValue = nullptr;
};

struct ListItem{
    mutable char Title[MAX_LISTITEM_NAME_NUM];
    ListItem * nextList;
    size_t nextListLength;
    std::function<void()> pFunc;
    ListItemExtra extra;
private:
    // 将 ListItem 内部的动画值也改为定点数
    int32_t anim_val1 = 0;
    int32_t anim_val2 = 0;
public:
    ListItem(const char* title,
             ListItem* next = nullptr,
             size_t nextLen = 0,
             std::function<void()> func = nullptr,
             ListItemExtra ex = {})
        : nextList(next), nextListLength(nextLen), pFunc(func), extra(ex)
    {
        strncpy(Title, title, sizeof(Title));
        Title[sizeof(Title)-1] = 0;
    }
    // int32_t getVal1 () const {return anim_val1;}
    // int32_t getVal2 () const {return anim_val2;}
    // void updateExtra() {}
};

class ListView : public IApplication {
public:
    ListView(PixelUI& ui, ListItem *itemList, size_t length) : m_ui(ui), m_itemList(itemList), m_itemLength(length - 1) {}
    ~ListView() = default;

    void draw() override;
    bool handleInput(InputEvent event) override;
    void onEnter(ExitCallback exitCallback) override;
    void onResume() override ;
    void onPause() override;
    void onExit() override;
    void resizeLength(size_t itemLength) { m_itemLength = itemLength; }
    PixelUI& getUI() { return m_ui; }
    
    PixelUI& m_ui;
private:
    ListItem* m_itemList;
    size_t m_itemLength;
    uint8_t spacing_ = 3;
    uint8_t topMargin_ = 2;
    uint8_t FontHeight = 0;
    
    etl::vector<etl::pair<etl::pair<ListItem*, size_t>, size_t>, MAX_LISTVIEW_DEPTH> m_history_stack;

    // 光标相关
    int32_t CursorY = -6;
    int32_t CursorX = 1;
    int32_t CursorWidth = 0;
    
    // 滚动相关
    int32_t scrollOffset_ = 0; // 修改为 int32_t
    int topVisibleIndex_ = 0;
    int visibleItemCount_ = LISTVIEW_ITEMS_PER_PAGE;
    
    // 载入动画相关
    int32_t itemLoadAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1]; // 修改为 int32_t 数组
    bool isInitialLoad_ = true;
    int32_t animation_pixel_dots = 0; // 修改为 int32_t
    int32_t animation_scroll_bar = 0; // 修改为 int32_t
    
    // 过渡动画相关
    bool isTransitioning_ = false;
    int32_t transitionProgress_ = 0;
    int selectedItemForTransition_ = -1;
    int32_t itemExitAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1];
    int32_t itemEnterAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1];
    int32_t selectedItemY_ = 0;
    ListItem* oldItemList_ = nullptr;
    size_t oldItemLength_ = 0;
    int oldTopVisibleIndex_ = 0;
    int32_t switchBoxX = 0;

    // progess bar related
    int32_t progress_bar_top = 0;
    int32_t progress_bar_bottom = 0;

    void navigateLeft();
    void navigateRight();
    void navigateUp();
    void navigateDown();

    void drawCursor();
    void scrollToTarget(size_t target);
    void updateScrollPosition();
    void startLoadAnimation();
    void startTransitionAnimation(int selectedItemIndex);
    int getVisibleItemIndex(int screenIndex);
    bool shouldScroll(int newCursor);
    int32_t calculateItemY(int itemIndex);
    
    void selectCurrent();
    void returnToPreviousContext();

    void clearNonInitialAnimations();
    
    size_t currentCursor = 0;
};