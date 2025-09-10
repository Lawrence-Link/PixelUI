#include "PixelUI/pixelui.h"
#include "PixelUI/core/ViewManager/ViewManager.h"
#include <cassert>
#include "EmuWorker.h"
#include <functional>

PixelUI::PixelUI(U8G2& u8g2) : u8g2_(u8g2), _currentTime(0) {
    // popupManager_ = std::make_unique<PopupManager>(*this, _animationManager);
    m_viewManagerPtr = std::make_shared<ViewManager>(*this);
    m_animationManagerPtr = std::make_shared<AnimationManager>();
}

void PixelUI::begin() 
{
    _currentTime = 0;
    m_animationManagerPtr->clear();
    // _totalAnimationsCreated = 0;
    // _animationUpdateCalls = 0;
}

void PixelUI::Heartbeat(uint32_t ms) 
{
    _currentTime += ms;
    // _animationUpdateCalls++;
    
    size_t beforeCount = m_animationManagerPtr->activeCount();
    m_animationManagerPtr->update(_currentTime);
    size_t afterCount = m_animationManagerPtr->activeCount();
    
    // 更新popup
    if (popupManager_) {
        popupManager_->update(_currentTime);
    }
}

void PixelUI::addAnimation(std::shared_ptr<Animation> animation) {
    animation->start(_currentTime); // 启动动画
    m_animationManagerPtr->addAnimation(animation); // 交给动画管理器
}

void PixelUI::animate(float& value, float targetValue, uint32_t duration, EasingType easing, PROTECTION prot) {
    auto animation = std::make_shared<CallbackAnimation>(
        value, targetValue, duration, easing,
        [&value](float currentValue) {
            value = currentValue;
        }
    );
    if (prot == PROTECTION::PROTECTED) m_animationManagerPtr->markProtected(animation);
    addAnimation(animation);
}

void PixelUI::animate(float& x, float& y, float targetX, float targetY, uint32_t duration, EasingType easing, PROTECTION prot) {
    // 为 X 坐标创建动画
    auto animX = std::make_shared<CallbackAnimation>(
        x, targetX, duration, easing,
        [&x](float currentValue) {
            x = currentValue;
        }
    );
    addAnimation(animX);

    // 为 Y 坐标创建动画
    auto animY = std::make_shared<CallbackAnimation>(
        y, targetY, duration, easing,
        [&y](float currentValue) {
            y = currentValue;
        }
    );
    addAnimation(animY);

    if (prot == PROTECTION::PROTECTED) {
        m_animationManagerPtr->markProtected(animX);
        m_animationManagerPtr->markProtected(animY);
    }
}

void PixelUI::renderer() {
    if (!isFading_){
        this->getU8G2().clearBuffer();
        
        if (currentDrawable_ && isDirty()) {
            currentDrawable_->draw();
            isDirty_ = false;
        }
        
        // 绘制popup
        if (popupManager_) {
            popupManager_->draw();
        }
        
        this->getU8G2().sendBuffer();
    } else { // isFading
        uint8_t * buf_ptr = this->getU8G2().getBufferPtr();
        uint16_t buf_len = 1024;
        for (int fade = 1; fade <= 4; fade++){
            switch (fade)
            {
                case 1: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0xAA; break;
                case 2: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
                case 3: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x55; break;
                case 4: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
            }
            this->getU8G2().sendBuffer();
            getEmuRefreshFunction()();
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
        isFading_ = false;
    }
}