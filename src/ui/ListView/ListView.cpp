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

#include "ui/ListView/ListView.h"
#include "core/animation/animation.h"

void ListView::onEnter(ExitCallback exitCallback){
    IApplication::onEnter(exitCallback);
    m_ui.setContinousDraw(true);
    U8G2& u8g2 = m_ui.getU8G2();

    u8g2.setFont(u8g2_font_squeezed_b6_tr);
    FontHeight = u8g2.getFontAscent() - u8g2.getFontDescent();
    
    topVisibleIndex_ = 0;
    scrollOffset_ = 0;
    currentCursor = 0;
    isInitialLoad_ = true;
    
    for (int i = 0; i < visibleItemCount_; i++) {
        itemLoadAnimations_[i] = 0;
    }
    
    // animation: scrollbar
    m_ui.animate(animation_pixel_dots, 32, 400, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);

    startLoadAnimation();
    scrollToTarget(0);
}
/*
@brief Called when the ListView is resumed from a paused state.
*/
void ListView::startLoadAnimation() {
    isInitialLoad_ = true;
    
    int maxVisible = std::min(visibleItemCount_ + 1, (int)(m_itemLength + 1));
    
    for (int i = 0; i < maxVisible; i++) {
        int duration = 250 + i * 60;
        
        bool isLastAnimation = (i == maxVisible - 1);
        
        auto callback = [this, i, isLastAnimation](int32_t value) {
            this->itemLoadAnimations_[i] = value;
            
            if (isLastAnimation && value >= FIXED_POINT_ONE) { 
                this->isInitialLoad_ = false;
                this->m_ui.getAnimationManPtr()->clearAllProtectionMarks();
            }
        };

        auto animation = std::make_shared<CallbackAnimation>(0, FIXED_POINT_ONE, duration, EasingType::EASE_IN_OUT_CUBIC, callback);
        
        m_ui.getAnimationManPtr()->markProtected(animation);
        m_ui.addAnimation(animation);
    }
}

void ListView::clearNonInitialAnimations() {
    m_ui.getAnimationManPtr()->clearUnprotected();
}

/*
@brief determine if scrolling is needed based on the new cursor position.
@param newCursor the new cursor position.
@return true if scrolling is needed, false otherwise.
*/
bool ListView::shouldScroll(int newCursor) {
    return (newCursor < topVisibleIndex_ || newCursor >= topVisibleIndex_ + visibleItemCount_);
}
/*
@brief update the scroll position based on the current cursor.
@param target the target item index to scroll to.
*/
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
        int32_t targetScrollOffset = -newTopIndex * (FontHeight + spacing_);
        m_ui.animate(scrollOffset_, targetScrollOffset, 350, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
        topVisibleIndex_ = newTopIndex;
    }
}

int32_t ListView::calculateItemY(int itemIndex) {
    U8G2& u8g2 = m_ui.getU8G2();
    int32_t baseY = topMargin_ + itemIndex * (FontHeight + spacing_) + u8g2.getFontAscent();
    return baseY + scrollOffset_;
}

void ListView::scrollToTarget(size_t target){
    updateScrollPosition();
    
    U8G2& u8g2 = m_ui.getU8G2();
    int screenCursorIndex = currentCursor - topVisibleIndex_;
    int32_t targetCursorY = topMargin_ + screenCursorIndex * (FontHeight + spacing_) - 1;
    
    //  Y coordinate of the cursor 
    m_ui.animate(CursorY, targetCursorY, 150, EasingType::EASE_IN_OUT_CUBIC);
    // Width of the cursor
    m_ui.animate(CursorWidth, u8g2.getUTF8Width(m_itemList[currentCursor].Title) + 6, 500, EasingType::EASE_OUT_CUBIC);
    // Top of the progress bar
    m_ui.animate(progress_bar_top, ((int64_t)currentCursor * 64) / (m_itemLength + 1) + 1, 400, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED); // 修正为定点数运算
    // Bottom of the progress bar
    m_ui.animate(progress_bar_bottom, ((int64_t)1 * 64) / (m_itemLength + 1), 400, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED); // 修正为定点数运算
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
    // one without nextlist, but with function
    if (!m_itemList[currentCursor].nextList && m_itemList[currentCursor].pFunc ){ m_itemList[currentCursor].pFunc(); } // 进入 pFunc
    
    else if (m_itemList[currentCursor].extra.switchValue) {
        bool* switchValPtr = m_itemList[currentCursor].extra.switchValue;

        // current state of the switch
        bool currentState = *switchValPtr;

        // calculate the start and end positions for the switch box
        int32_t startX = currentState ? 7 : 0;
        int32_t endX = currentState ? 0 : 7;
        
        // animate switchBoxX ( the dense box inside the switch frame act as the switch button )
        m_ui.animate(switchBoxX, endX, 200, EasingType::EASE_IN_OUT_CUBIC);

        // switch the value after the animation duration
        *switchValPtr = !currentState;
        return;
    }
    else { 
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

/*
@brief Return to the previous context in the history stack or exit if none exists.
*/
void ListView::returnToPreviousContext() {
    if (!m_history_stack.empty()){
        m_ui.getAnimationManPtr()->clear(); // stop all animations
        etl::pair<etl::pair<ListItem*, size_t>, size_t> parent_state = m_history_stack.back(); // get the last state
        m_history_stack.pop_back(); // remove it from the stack
        // extract the ListItem* and length
        m_itemList = parent_state.first.first;
        m_itemLength = parent_state.first.second;
        currentCursor = parent_state.second;
        
        // markFading and start load animation
        m_ui.markFading();
        startLoadAnimation();
        scrollToTarget(currentCursor);
        return;
    }
    else { requestExit(); } // exit if no history
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
    u8g2.setFont(u8g2_font_squeezed_b6_tr); 

    int startIndex = std::max(0, topVisibleIndex_ - 2);
    int endIndex = std::min((int)m_itemLength, topVisibleIndex_ + visibleItemCount_ + 2);
    
    for (int itemIndex = startIndex; itemIndex <= endIndex; itemIndex++) {
        int32_t itemY = calculateItemY(itemIndex);
        
        if (itemY >= -FontHeight && itemY <= u8g2.getDisplayHeight() + FontHeight) {
            int32_t drawX = 4;
            
            if (isInitialLoad_) {
                int animIndex = itemIndex - topVisibleIndex_;
                if (animIndex >= 0 && animIndex < visibleItemCount_ + 1) {
                    int32_t loadProgress = itemLoadAnimations_[animIndex];
                    drawX = 4 + (FIXED_POINT_ONE - loadProgress) * 30 / FIXED_POINT_ONE;
                }
            }
            u8g2.drawStr(drawX, itemY, m_itemList[itemIndex].Title);
            
            if (m_itemList[itemIndex].extra.switchValue) {
                u8g2.drawRFrame(u8g2.getDisplayWidth() - 35, itemY - 6, 14, 7, 1);
                u8g2.drawRBox(u8g2.getDisplayWidth() - 35 + switchBoxX, itemY - 6, 7, 7, 2);
                if (*m_itemList[itemIndex].extra.switchValue)
                    u8g2.drawStr(u8g2.getDisplayWidth() - 18, itemY, "ON");
                else 
                    u8g2.drawStr(u8g2.getDisplayWidth() - 18, itemY, "OFF");
            }

            if (m_itemList[itemIndex].extra.intValue) {
                char buf[5] = {0};
                snprintf(buf, 5, "%d", *m_itemList[itemIndex].extra.intValue);
                u8g2.drawStr(u8g2.getDisplayWidth() - 18, itemY, buf);
            }
        }
    }

    u8g2.drawVLine(126, progress_bar_top, progress_bar_bottom);
    drawCursor();
}

int ListView::getVisibleItemIndex(int screenIndex) {
    return topVisibleIndex_ + screenIndex;
}