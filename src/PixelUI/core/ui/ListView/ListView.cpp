#include "PixelUI/core/ui/ListView/ListView.h"

void ListView::onEnter(ExitCallback exitCallback){
    IApplication::onEnter(exitCallback);
    m_ui.setContinousDraw(true);
    U8G2& u8g2 = m_ui.getU8G2();

    u8g2.setFont(u8g2_font_squeezed_b6_tr);
    FontHeight = u8g2.getFontAscent() - u8g2.getFontDescent();
    
    topVisibleIndex_ = 0;
    scrollOffset_ = 0.0f;
    currentCursor = 0;
    isInitialLoad_ = true;
    
    for (int i = 0; i < visibleItemCount_; i++) {
        itemLoadAnimations_[i] = 0.0f;
    }
    
    // animation: scrollbar
    m_ui.animate(animation_pixel_dots, 32, 400, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);

    startLoadAnimation();
    scrollToTarget(0);
}

void ListView::startLoadAnimation() {
    isInitialLoad_ = true;
    
    int maxVisible = std::min(visibleItemCount_ + 1, (int)(m_itemLength + 1));
    
    for (int i = 0; i < maxVisible; i++) {
        int duration = 250 + i * 60;
        
        bool isLastAnimation = (i == maxVisible - 1);
        
        auto callback = [this, i, isLastAnimation](float value) {
            this->itemLoadAnimations_[i] = value;
            
            if (isLastAnimation && value >= 1.0f) {
                this->isInitialLoad_ = false;
                this->m_ui.getAnimationManPtr()->clearAllProtectionMarks();
            }
        };

        auto animation = std::make_shared<CallbackAnimation>(0.0f, 1.0f, duration, EasingType::EASE_IN_OUT_CUBIC, callback);
        animation->start(m_ui.getCurrentTime());

        m_ui.getAnimationManPtr()->markProtected(animation);
        m_ui.getAnimationManPtr()->addAnimation(animation);
    }
}

void ListView::clearNonInitialAnimations() {
    m_ui.getAnimationManPtr()->clearUnprotected();
}

bool ListView::shouldScroll(int newCursor) {
    return (newCursor < topVisibleIndex_ || newCursor >= topVisibleIndex_ + visibleItemCount_);
}

void ListView::updateScrollPosition() {
    if (!shouldScroll(currentCursor)) {
        return;
    }
    // if scroll is needed then:
    // calculate new top visible index
    int newTopIndex = topVisibleIndex_;
    if (currentCursor < topVisibleIndex_) {
        newTopIndex = currentCursor;
    } else if (currentCursor >= topVisibleIndex_ + visibleItemCount_) {
        newTopIndex = currentCursor - visibleItemCount_ + 1;
    }
    
    int maxTopIndex = std::max(0, (int)m_itemLength + 1 - visibleItemCount_);
    newTopIndex = std::max(0, std::min(newTopIndex, maxTopIndex));
    
    if (newTopIndex != topVisibleIndex_) {
        float targetScrollOffset = -newTopIndex * (FontHeight + spacing_);
        m_ui.animate(scrollOffset_, targetScrollOffset, 350, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
        topVisibleIndex_ = newTopIndex;
    }
}

float ListView::calculateItemY(int itemIndex) {
    U8G2& u8g2 = m_ui.getU8G2();
    float baseY = topMargin_ + itemIndex * (FontHeight + spacing_) + u8g2.getFontAscent();
    return baseY + scrollOffset_;
}

void ListView::scrollToTarget(size_t target){
    updateScrollPosition();
    
    U8G2& u8g2 = m_ui.getU8G2();
    int screenCursorIndex = currentCursor - topVisibleIndex_;
    float targetCursorY = topMargin_ + screenCursorIndex * (FontHeight + spacing_) - 1;
    
    // 光标的Y轴
    m_ui.animate(CursorY, targetCursorY, 240, EasingType::EASE_IN_OUT_CUBIC);
    // 反色光标的宽度
    m_ui.animate(CursorWidth, m_ui.getU8G2().getUTF8Width(m_itemList[currentCursor].Title) + 6, 500, EasingType::EASE_OUT_CUBIC);
    // 进度条顶端
    m_ui.animate(progress_bar_top, ((float)currentCursor/((float)m_itemLength + 1)) * 64.0f + 1, 400, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    // 进度条底部
    m_ui.animate(progress_bar_bottom, (1/((float)m_itemLength + 1)) * 64.0f - 1, 400, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
}

void ListView::navigateUp() {
    if (currentCursor != 0) {
        clearNonInitialAnimations();
    }
    if (currentCursor > 0) {
        currentCursor--;
        scrollToTarget(currentCursor);
    }
}

void ListView::navigateDown() {
    if (currentCursor != m_itemLength) {
        clearNonInitialAnimations();
    }
    if (currentCursor < m_itemLength) {
        currentCursor++;
        scrollToTarget(currentCursor);
    }
}

void ListView::selectCurrent(){
    if (currentCursor == 0) {
        returnToPreviousContext();
        return ;
    }
    // 无nextlist 有function 无int/switch value
    if (!m_itemList[currentCursor].nextList && m_itemList[currentCursor].pFunc && !m_itemList[currentCursor].extra.intValue && !m_itemList[currentCursor].extra.switchValue){ m_itemList[currentCursor].pFunc(); } // 进入 pFunc
    else if (m_itemList[currentCursor].extra.intValue) { // 有int value
        *m_itemList[currentCursor].extra.intValue = !(*m_itemList[currentCursor].extra.intValue);
        // switch (*m_itemList[currentCursor].extra.intValue) {
        //     case false: m_itemList[currentCursor];
        //     case true: break;
        // }
    }
    else { 
        // clearNonInitialAnimations();
        m_ui.getAnimationManPtr()->clear();
        m_history_stack.push_back(etl::make_pair(etl::make_pair(m_itemList, m_itemLength), currentCursor));
        m_itemLength = m_itemList[currentCursor].nextListLength - 1;
        m_itemList = m_itemList[currentCursor].nextList;
        currentCursor = 0;
        m_ui.markFading();
        startLoadAnimation();
        
        scrollToTarget(0);
        return;
    }
}

void ListView::returnToPreviousContext() {
    if (!m_history_stack.empty()){
        // clearNonInitialAnimations();
        m_ui.getAnimationManPtr()->clear();
        etl::pair<etl::pair<ListItem*, size_t>, size_t> parent_state = m_history_stack.back();
        m_history_stack.pop_back();
        m_itemList = parent_state.first.first;
        m_itemLength = parent_state.first.second;
        currentCursor = parent_state.second;
        m_ui.markFading();
        startLoadAnimation();
        scrollToTarget(currentCursor);
        return;
    }
    else { requestExit(); }
}

void ListView::navigateLeft() {
    returnToPreviousContext();
}
void ListView::navigateRight() {
    selectCurrent();
}

bool ListView::handleInput(InputEvent event) {
    switch (event) {
        case InputEvent::UP: navigateUp(); return true;
        case InputEvent::DOWN: navigateDown(); return true;
        case InputEvent::LEFT:  navigateLeft(); return true;
        case InputEvent::RIGHT: navigateRight(); return true;
        case InputEvent::SELECT: return true;
        case InputEvent::BACK: requestExit(); return true;
        default: return false;
    }
    return false;
}

void ListView::drawCursor() {
    U8G2& u8g2 = m_ui.getU8G2();
    u8g2.setDrawColor(2);
    u8g2.drawRBox(CursorX, CursorY, CursorWidth, FontHeight + 2, 0);
    u8g2.setDrawColor(1);

    if (!currentCursor)
        u8g2.drawStr(u8g2.getDisplayWidth() - u8g2.getUTF8Width("BACK") - 5, u8g2.getDisplayHeight() - 5, "BACK");
    else
        u8g2.drawStr(u8g2.getDisplayWidth() - u8g2.getUTF8Width(">>") - 5, u8g2.getDisplayHeight() - 5, ">>");
}

void ListView::onResume() {
    isInitialLoad_ = false;
    m_ui.getAnimationManPtr()->clearAllProtectionMarks();
}

void ListView::onPause() {}

void ListView::onExit() {
    m_ui.markFading();
    m_ui.setContinousDraw(false);
    m_ui.getAnimationManPtr()->clearAllProtectionMarks();
}

void ListView::draw(){
    U8G2& u8g2 = m_ui.getU8G2();
    
    int startIndex = std::max(0, topVisibleIndex_ - 2);
    int endIndex = std::min((int)m_itemLength, topVisibleIndex_ + visibleItemCount_ + 2);
    
    for (int itemIndex = startIndex; itemIndex <= endIndex; itemIndex++) {
        float itemY = calculateItemY(itemIndex);
        
        if (itemY >= -FontHeight && itemY <= u8g2.getDisplayHeight() + FontHeight) {
            float drawX = 4;
            
            if (isInitialLoad_) {
                int animIndex = itemIndex - topVisibleIndex_;
                if (animIndex >= 0 && animIndex < visibleItemCount_ + 1) {
                    float loadProgress = itemLoadAnimations_[animIndex];
                    drawX = 4 + (1.0f - loadProgress) * 30;
                }
            }
            u8g2.drawStr(drawX, itemY, m_itemList[itemIndex].Title);
            
            if (m_itemList[itemIndex].extra.switchValue) {
                u8g2.drawFrame(u8g2.getDisplayWidth() - 30, itemY - 6, 14, 7);
                // u8g2.drawBox(u8g2.getDisplayWidth() - 30 + m_itemList[itemIndex].getVal1()/* +7 or 0 */, itemY - 6, 7, 7);
            }

            if (m_itemList[itemIndex].extra.intValue) {
                
            }
        }
    }

    /* Draw the side progress bar */

    // for (int i = 0; i <= static_cast<int> (animation_pixel_dots); i++)
    //     u8g2.drawPixel(126, 2 * i);

    // display.drawBox(0, 49, animation_scroll_bar, 3);
    // u8g2.drawVLine(u8g2.getDisplayWidth() - 2, 50, animation_scroll_bar);

    u8g2.drawVLine(126, progress_bar_top, progress_bar_bottom);

    drawCursor();
}