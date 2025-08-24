#include "PixelUI/core/ui/ListView/ListView.h"

// ... (image_LISTVIEW_bits 和 ItemList 定义保持不变) ...
static const unsigned char image_LISTVIEW_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x7c,0xe3,0xff,0xff,0xf7,0x07,0x7f,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x78,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x7c,0xe3,0xff,0xff,0xff,0xdf,0x45,0xfc,0xdf,0xe5,0xfe,0x1e,0xcd,0x7e,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

void ListView::onEnter(ExitCallback exitCallback){
    IApplication::onEnter(exitCallback);
    m_ui.setContinousDraw(true);
    U8G2& u8g2 = m_ui.getU8G2();

    u8g2.setFont(u8g2_font_squeezed_b6_tr);
    FontHeight = u8g2.getFontAscent() - u8g2.getFontDescent();
    
    topVisibleIndex_ = 0;
    scrollOffset_ = 0.0f;
    currentCursor = 0;
    isInitialLoad_ = true; // 动画开始前，设置为true
    
    for (int i = 0; i < visibleItemCount_; i++) {
        itemLoadAnimations_[i] = 0.0f;
    }
    
    startLoadAnimation();
    scrollToTarget(0);
}

void ListView::startLoadAnimation() {
    int maxVisible = std::min(visibleItemCount_ + 1, (int)(m_itemLength + 1));
    
    for (int i = 0; i < maxVisible; i++) {
        int duration = 250 + i * 100;
        
        bool isLastAnimation = (i == maxVisible - 1);
        
        auto callback = [this, i, isLastAnimation](float value) {
            this->itemLoadAnimations_[i] = value;
            
            if (isLastAnimation && value >= 1.0f) {
                this->isInitialLoad_ = false;
                std::cout << "[DEBUG] Initial load animation sequence complete." << std::endl;
            }
        };
        
        auto animation = std::make_shared<CallbackAnimation>(0.0f, 1.0f, duration, EasingType::EASE_IN_OUT_CUBIC, callback);
        
        // 关键修复：启动动画
        animation->start(m_ui.getCurrentTime()); // 需要获取当前时间
        
        m_ui.getAnimationMan().addAnimation(animation);
    }
}

// ... (shouldScroll, updateScrollPosition, calculateItemY, scrollToTarget, navigate*, handleInput, drawCursor 函数都保持不变) ...

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
        isInitialLoad_ = false; // 滚动时也禁用初始动画
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
    
    m_ui.animate(CursorY, targetCursorY, 140, EasingType::EASE_IN_OUT_CUBIC);
    m_ui.animate(CursorWidth, m_ui.getU8G2().getUTF8Width(ItemList[currentCursor].Title) + 6, 500, EasingType::EASE_OUT_CUBIC);
}

void ListView::navigateUp() {
    if (currentCursor != 0)
        m_ui.getAnimationMan().clear();
    if (currentCursor > 0) {
        currentCursor--;
        scrollToTarget(currentCursor);
    }
}

void ListView::navigateDown() {
    if (currentCursor != m_itemLength)
        m_ui.getAnimationMan().clear();
    if (currentCursor < m_itemLength) {
        currentCursor++;
        scrollToTarget(currentCursor);
    }
}

void ListView::navigateLeft() {}
void ListView::navigateRight() {}

bool ListView::handleInput(InputEvent event) {
    switch (event) {
        case InputEvent::UP: navigateUp(); return true;
        case InputEvent::DOWN: navigateDown(); return true;
        case InputEvent::LEFT:  navigateLeft(); return true;
        case InputEvent::RIGHT: navigateRight(); return true;
        case InputEvent::SELECT: /*TBD*/ return true;
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

    if (ItemList[currentCursor].isTitle)
        u8g2.drawStr(u8g2.getDisplayWidth() - u8g2.getUTF8Width("BACK") - 5, u8g2.getDisplayHeight() - 5, "BACK");
    else
        u8g2.drawStr(u8g2.getDisplayWidth() - u8g2.getUTF8Width(">>") - 5, u8g2.getDisplayHeight() - 5, ">>");
}

void ListView::onResume() {
    isInitialLoad_ = false; // 从其他视图返回时，不播放初始动画
}

void ListView::onPause() {}

void ListView::onExit() {
    m_ui.markFading();
    m_ui.setContinousDraw(false);
}

void ListView::draw(){
    U8G2& u8g2 = m_ui.getU8G2();
    
    int startIndex = std::max(0, topVisibleIndex_ - 2);
    int endIndex = std::min((int)m_itemLength, topVisibleIndex_ + visibleItemCount_ + 2);
    
    for (int itemIndex = startIndex; itemIndex <= endIndex; itemIndex++) {
        float itemY = calculateItemY(itemIndex);
        
        if (itemY >= -FontHeight && itemY <= u8g2.getDisplayHeight() + FontHeight) {
            float drawX = 4; // 默认最终位置
            
            // 只有在初始加载状态下才执行动画计算
            if (isInitialLoad_) {
                int animIndex = itemIndex - topVisibleIndex_;
                if (animIndex >= 0 && animIndex < visibleItemCount_ + 1) {
                    float loadProgress = itemLoadAnimations_[animIndex];
                    drawX = 4 + (1.0f - loadProgress) * 30;
                }
            }
            u8g2.drawStr(drawX, itemY, ItemList[itemIndex].Title);
        }
    }
    drawCursor();
}

static AppRegistrar registrar_about_app({
    .title = "ListView Test",
    .bitmap = image_LISTVIEW_bits,
    
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> { 
        return std::make_unique<ListView>(ui, ItemList, sizeof(ItemList)/sizeof(ItemList[0])); 
    },
    
    .type = MenuItemType::App,
    .w = 24, 
    .h = 24
});