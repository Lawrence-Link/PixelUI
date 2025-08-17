#pragma once

#include <iostream>
#include "PixelScriptUI/core/animation/anime.h"
#include "PixelScriptUI/core/menu/menu_system.h"
#include <u8g2_wrapper.h>

enum class Screen{
    TileMenu,
    ListMenu,
    APP
};

class PixelUI
{
public:
    PixelUI(U8G2Wrapper& u8g2) : u8g2_(u8g2), _currentTime(0) {};
    ~PixelUI() = default;

    void begin(Screen s);
    void Heartbeat(uint32_t ms);
    
    void animate(float& value, float targetValue, uint32_t duration, EasingType easing = EasingType::LINEAR);
    void animate(float& x, float& y, float targetX, float targetY, uint32_t duration, EasingType easing = EasingType::LINEAR);
    void addAnimation(std::shared_ptr<Animation> animation);

    bool isPointerValid(const void* ptr) const;
    void debugInfo() const;

    uint32_t getCurrentTime() const { return _currentTime; }
    uint32_t getActiveAnimationCount() const { return _animationManager.activeCount(); };

protected:
private:
    U8G2Wrapper& u8g2_;
    AnimationManager _animationManager;
    uint32_t _currentTime;
    
    // 添加调试统计
    mutable size_t _totalAnimationsCreated = 0;
    mutable size_t _animationUpdateCalls = 0;
};

