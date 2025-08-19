#pragma once

#include <iostream>
#include "PixelUI/core/animation/anime.h"
// #include "PixelUI/core/menu/menu_system.h"
#include "PixelUI/core/app/app_system.h"
#include <u8g2_wrapper.h>

enum class ContentType{
    TileMenu,
    ListMenu,
    APP
};

struct contentType{
    ContentType type;
    void*       content;
};

class IDrawable {
public:
    virtual void draw() = 0;
    virtual ~IDrawable() = default; 
    virtual void update(uint32_t currentTime) {};
};

class PixelUI
{
public:
    PixelUI(U8G2Wrapper& u8g2) : u8g2_(u8g2), _currentTime(0) {};
    ~PixelUI() = default;

    void begin();
    void Heartbeat(uint32_t ms);
    
    U8G2Wrapper& getU8G2() { return u8g2_; }
    const U8G2Wrapper& getU8G2() const { return u8g2_; }

    void animate(float& value, float targetValue, uint32_t duration, EasingType easing = EasingType::LINEAR);
    void animate(float& x, float& y, float targetX, float targetY, uint32_t duration, EasingType easing = EasingType::LINEAR);
    void addAnimation(std::shared_ptr<Animation> animation);

    void setDrawable(IDrawable* drawable) {
        currentDrawable_ = drawable;
        markDirty();
    }

    IDrawable* getDrawable() const { return currentDrawable_; };

    void renderer();

    void markDirty() { isDirty_ = true; }
    
    bool isDirty() const { return isDirty_; }
    bool isPointerValid(const void* ptr) const;

    uint32_t getCurrentTime() const { return _currentTime; }
    uint32_t getActiveAnimationCount() const { return _animationManager.activeCount(); };

protected:
private:
    U8G2Wrapper& u8g2_;
    AnimationManager _animationManager;
    uint32_t _currentTime;
    IDrawable* currentDrawable_;
    bool isDirty_ = false;

    mutable size_t _totalAnimationsCreated = 0;
    mutable size_t _animationUpdateCalls = 0;
};