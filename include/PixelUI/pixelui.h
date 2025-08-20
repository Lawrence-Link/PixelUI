#pragma once

#include <iostream>
#include "PixelUI/core/animation/anime.h"
// #include "PixelUI/core/menu/menu_system.h"
// #include "PixelUI/core/app/app_system.h"
#include <u8g2_wrapper.h>

enum class InputEvent {
    UP, DOWN, LEFT, RIGHT, SELECT, BACK
};

class IDrawable {
public:
    virtual void draw() = 0;
    virtual ~IDrawable() = default; 
    virtual void update(uint32_t currentTime) {};
};

class IInputHandler{
public:
    virtual bool handleInput(InputEvent event) = 0;
    virtual ~IInputHandler() = default;
};

using InputCallback = std::function<bool(InputEvent)>;

class PixelUI
{
public:
    PixelUI(U8G2Wrapper& u8g2) : u8g2_(u8g2), _currentTime(0) {};
    ~PixelUI() = default;

    void begin();
    void Heartbeat(uint32_t ms);
    
    U8G2Wrapper& getU8G2() { return u8g2_; }
    const U8G2Wrapper& getU8G2() const { return u8g2_; }
    AnimationManager& getAnimationMan() { return _animationManager; }

    void animate(float& value, float targetValue, uint32_t duration, EasingType easing = EasingType::LINEAR);
    void animate(float& x, float& y, float targetX, float targetY, uint32_t duration, EasingType easing = EasingType::LINEAR);
    void addAnimation(std::shared_ptr<Animation> animation);

    void setDrawable(std::shared_ptr<IDrawable> drawable) {
        currentDrawable_ = drawable;
        markDirty();
    }

    std::shared_ptr<IDrawable> getDrawable() const { return currentDrawable_; };

    void renderer();

    void markDirty() { isDirty_ = true; }
    
    bool isDirty() const { return isDirty_; }
    bool isPointerValid(const void* ptr) const;

    uint32_t getCurrentTime() const { return _currentTime; }
    uint32_t getActiveAnimationCount() const { return _animationManager.activeCount(); };

    void setInputCallback(InputCallback callback) { inputCallback_ = callback; }
    bool handleInput(InputEvent event) {
        if (inputCallback_) return inputCallback_(event);
        return false;
    }

protected:
private:
    U8G2Wrapper& u8g2_;
    AnimationManager _animationManager;
    uint32_t _currentTime;
    std::shared_ptr<IDrawable> currentDrawable_;
    bool isDirty_ = false;
    InputCallback inputCallback_;

    mutable size_t _totalAnimationsCreated = 0;
    mutable size_t _animationUpdateCalls = 0;
};