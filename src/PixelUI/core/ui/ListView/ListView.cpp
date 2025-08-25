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
    
    startLoadAnimation();
    scrollToTarget(0);
}

void ListView::startLoadAnimation() {
    isInitialLoad_ = true;
    
    int maxVisible = std::min(visibleItemCount_ + 1, (int)(m_itemLength + 1));
    
    for (int i = 0; i < maxVisible; i++) {
        int duration = 250 + i * 100;
        
        bool isLastAnimation = (i == maxVisible - 1);
        
        auto callback = [this, i, isLastAnimation](float value) {
            this->itemLoadAnimations_[i] = value;
            
            if (isLastAnimation && value >= 1.0f) {
                this->isInitialLoad_ = false;
                this->m_ui.getAnimationMan().clearAllProtectionMarks();
                std::cout << "[DEBUG] Initial load animation sequence complete." << std::endl;
            }
        };
        
        auto animation = std::make_shared<CallbackAnimation>(0.0f, 1.0f, duration, EasingType::EASE_IN_OUT_CUBIC, callback);
        
        animation->start(m_ui.getCurrentTime());
        
        m_ui.getAnimationMan().markProtected(animation);
        m_ui.getAnimationMan().addAnimation(animation);
    }
}

void ListView::clearNonInitialAnimations() {
    if (isInitialLoad_) {
        m_ui.getAnimationMan().clearUnprotected();
        std::cout << "[DEBUG] Cleared non-initial animations while protecting initial load animations" << std::endl;
    } else {
        m_ui.getAnimationMan().clear();
        std::cout << "[DEBUG] Cleared all animations (not in initial load state)" << std::endl;
    }
}

bool ListView::shouldScroll(int newCursor) {
    return (newCursor < topVisibleIndex_ || newCursor >= topVisibleIndex_ + visibleItemCount_);
}

void ListView::updateScrollPosition() {
    if (!shouldScroll(currentCursor)) {
        return;
    }
    
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
        m_ui.animate(scrollOffset_, targetScrollOffset, 350, EasingType::EASE_OUT_CUBIC);
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
    
    m_ui.animate(CursorY, targetCursorY, 240, EasingType::EASE_IN_OUT_CUBIC);
    m_ui.animate(CursorWidth, m_ui.getU8G2().getUTF8Width(m_itemList[currentCursor].Title) + 6, 500, EasingType::EASE_OUT_CUBIC);
}

void ListView::navigateUp() {
    if (currentCursor != 0)
        clearNonInitialAnimations();
    if (currentCursor > 0) {
        currentCursor--;
        scrollToTarget(currentCursor);
    }
}

void ListView::navigateDown() {
    if (currentCursor != m_itemLength)
        clearNonInitialAnimations();
    if (currentCursor < m_itemLength) {
        currentCursor++;
        scrollToTarget(currentCursor);
    }
}

void ListView::selectCurrent(){
    if (currentCursor == 0) { 
    }
    if (!m_itemList[currentCursor].nextList){} 
    else { 
        clearNonInitialAnimations();
        m_history_stack.push_back(etl::make_pair(etl::make_pair(m_itemList, m_itemLength), currentCursor));
        m_itemLength = m_itemList[currentCursor].nextListLength - 1;
        m_itemList = m_itemList[currentCursor].nextList;
        currentCursor = 0;
        m_ui.markFading();
        startLoadAnimation();
        scrollToTarget(0);
        return;
    }

    if (currentCursor == 0) {
        if (!m_history_stack.empty()){
            clearNonInitialAnimations();
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
}

void ListView::navigateLeft() {}
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
    m_ui.getAnimationMan().clearAllProtectionMarks();
}

void ListView::onPause() {}

void ListView::onExit() {
    m_ui.markFading();
    m_ui.setContinousDraw(false);
    m_ui.getAnimationMan().clearAllProtectionMarks();
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
        }
    }
    drawCursor();
}