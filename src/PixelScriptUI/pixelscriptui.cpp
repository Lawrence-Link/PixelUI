// 在pixelscriptui.cpp中实现安全版本
#include "PixelScriptUI/pixelscriptui.h"
#include <iostream>
#include <cassert>

void PixelUI::begin(Screen s) 
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

bool PixelUI::isPointerValid(const void* ptr) const {
    return ptr != nullptr;
}

void PixelUI::debugInfo() const {
    std::cout << "[DEBUG] PixelUI State:" << std::endl;
    std::cout << "  Current time: " << _currentTime << std::endl;
    std::cout << "  Active animations: " << _animationManager.activeCount() << std::endl;
    std::cout << "  Total animations created: " << _totalAnimationsCreated << std::endl;
    std::cout << "  Update calls: " << _animationUpdateCalls << std::endl;
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