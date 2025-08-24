#pragma once

#include "PixelUI/core/ui/AppView/AppView.h"
#include <iostream>
#include <algorithm>
#include <cstring>

enum class terminus {
    ToNextNode,
    ToFunc
};

struct ListItem{ // Item prototype
    mutable char Title[MAX_LISTITEM_NAME_NUM];
    bool featIcon;
    bool isTitle;
    char * icon;
    terminus term;
    ListItem * nextItem;
    void (*pFunc)();
};

ListItem ItemList[] = { // text example
    { ">>> ListDemo <<<", false, true,  nullptr, terminus::ToFunc, nullptr, (void(*)())nullptr },
    { "- Bob then raped", false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item B\n"; } },
    { "- Cathy Flower",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Drunken",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Switch",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Alert",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Progress",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Anytone",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Potato",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Tomato",   false, false, nullptr, terminus::ToFunc, nullptr, (void(*)())[](){ std::cout << "Item C\n"; } },

};

class ListView : public IApplication {
public:
    ListView(PixelUI& ui, ListItem *itemList, size_t length) : m_ui(ui), m_itemList(ItemList), m_itemLength(length - 1) {}
    ~ListView() = default;

    void draw() override;
    bool handleInput(InputEvent event) override;
    void onEnter(ExitCallback exitCallback) override;
    void onResume() override ;
    void onPause() override;
    void onExit() override;
    void resizeLength(size_t itemLength) { m_itemLength = itemLength; }

private:
    PixelUI& m_ui;
    ListItem* m_itemList;
    size_t m_itemLength;
    uint8_t spacing_ = 3;      // <-- FIX 1: 使用固定的项目间距
    uint8_t topMargin_ = 2;    // <-- FIX 1: 增加固定的顶部边距
    uint8_t FontHeight = 0;
    
    // 光标相关
    float CursorY = 0;
    float CursorX = 1;
    float CursorWidth = 0;
    
    // 滚动相关
    float scrollOffset_ = 0.0f;
    int topVisibleIndex_ = 0;
    int visibleItemCount_ = LISTVIEW_ITEMS_PER_PAGE;
    
    // 载入动画相关
    float itemLoadAnimations_[LISTVIEW_ITEMS_PER_PAGE + 1];
    bool isInitialLoad_ = true;

    void navigateLeft();
    void navigateRight();
    void navigateUp();
    void navigateDown();

    void drawCursor();
    void scrollToTarget(size_t target);
    void updateScrollPosition();
    void startLoadAnimation();
    int getVisibleItemIndex(int screenIndex);
    bool shouldScroll(int newCursor);
    float calculateItemY(int itemIndex);

    size_t currentCursor = 0;
};