#pragma once

#include "PixelUI/core/ui/AppView/AppView.h"
#include "PixelUI/pixelui.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include "etl/vector.h"
#include "etl/delegate.h"

struct ListItem{
    mutable char Title[MAX_LISTITEM_NAME_NUM];
    ListItem * nextList;
    size_t nextListLength;
    // void (*pFunc)();
    std::function<void()> pFunc;
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
    float CursorY = -6;
    float CursorX = 1;
    float CursorWidth = 0;
    
    // 滚动相关
    float scrollOffset_ = 0.0f;
    int topVisibleIndex_ = 0;
    int visibleItemCount_ = LISTVIEW_ITEMS_PER_PAGE;
    
    // 载入动画相关
    float itemLoadAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1];
    bool isInitialLoad_ = true;
    float animation_pixel_dots = 0.0f;
    float animation_scroll_bar = 0.0f;
    
    // 过渡动画相关
    bool isTransitioning_ = false;
    float transitionProgress_ = 0.0f;
    int selectedItemForTransition_ = -1;
    float itemExitAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1];
    float itemEnterAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1];
    float selectedItemY_ = 0.0f;
    ListItem* oldItemList_ = nullptr;
    size_t oldItemLength_ = 0;
    int oldTopVisibleIndex_ = 0;

    // progess bar related
    float progress_bar_top = 0;
    float progress_bar_bottom = 0;

    void navigateLeft();
    void navigateRight();
    void navigateUp();
    void navigateDown();

    void drawCursor();
    void scrollToTarget(size_t target);
    void updateScrollPosition();
    void startLoadAnimation();
    void startTransitionAnimation(int selectedItemIndex);
    void startBackTransitionAnimation();
    int getVisibleItemIndex(int screenIndex);
    bool shouldScroll(int newCursor);
    float calculateItemY(int itemIndex);
    
    void selectCurrent();
    void returnToPreviousContext();

    // 选择性清除动画的方法
    void clearNonInitialAnimations();
    
    size_t currentCursor = 0;
};