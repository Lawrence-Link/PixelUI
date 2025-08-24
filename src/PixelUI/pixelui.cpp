#include "PixelUI/pixelui.h"
#include <iostream>
#include <cassert>
#include "EmuWorker.h"
#include <functional>

void PixelUI::begin() 
{
    std::cout << "[DEBUG] PixelUI::begin() called" << std::endl;
    _currentTime = 0;
    _animationManager.clear();
    _totalAnimationsCreated = 0;
    _animationUpdateCalls = 0;
    std::cout << "[DEBUG] PixelUI initialized successfully" << std::endl;
}

void PixelUI::Heartbeat(uint32_t ms) 
{
    // std::cout << "[DEBUG] Heartbeat called with ms=" << ms << ", currentTime=" << _currentTime << std::endl;
    
    _currentTime += ms;
    _animationUpdateCalls++;
    
    size_t beforeCount = _animationManager.activeCount();
    _animationManager.update(_currentTime);
    size_t afterCount = _animationManager.activeCount();
    
    // std::cout << "[DEBUG] Animations: " << beforeCount << " -> " << afterCount << std::endl;
}

void PixelUI::addAnimation(std::shared_ptr<Animation> animation) {
    animation->start(_currentTime); // 启动动画
    _animationManager.addAnimation(animation); // 交给动画管理器
}

void PixelUI::animate(float& value, float targetValue, uint32_t duration, EasingType easing) {
    auto animation = std::make_shared<CallbackAnimation>(
        value, targetValue, duration, easing,
        [&value](float currentValue) {
            value = currentValue;
        }
    );
    addAnimation(animation);
}

// 新增：位置坐标的安全动画实现
void PixelUI::animate(float& x, float& y, float targetX, float targetY, uint32_t duration, EasingType easing) {
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
}

void PixelUI::renderer() {
    if (!isFading_){
        this->getU8G2().clearBuffer();
        if (currentDrawable_ && isDirty()) {
            currentDrawable_->draw();
            isDirty_ = false;
        }
        this->getU8G2().sendBuffer();
    } else { // isFading
        uint8_t * buf_ptr = this->getU8G2().getBufferPtr();
        uint16_t buf_len = 1024;
        for (int fade = 1; fade <= 4; fade++){
            switch (fade)
            {
                case 1: for (uint16_t i = 0; i < buf_len; ++i)  
                            if (i % 2 != 0) 
                                buf_ptr[i] = buf_ptr[i] & 0xAA;
                        break;
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