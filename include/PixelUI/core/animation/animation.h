#pragma once

#include <iostream>
#include <cstdint>
#include <cmath>
#include <memory>
#include "etl/vector.h"
#include <functional>
#include "PixelUI/config.h"
#include "PixelUI/core/CommonTypes.h"
#include "PixelUI/core/animation/animation.h"

class EasingCalculator {
public:
    static float calculate(EasingType type, float t);
private:
    static float easeOutBounce(float t);
};

class Animation {
public:
    Animation(uint32_t duration, EasingType easing = EasingType::LINEAR) :
    _duration(duration),
    _easing(easing), _startTime(0), _isActive(0), _progress(0.0f) {};

    virtual ~Animation() = default;

    void start(uint32_t currentTime);
    void stop();
    virtual bool update(uint32_t currentTime);

    bool isActive() const { return _isActive; }
    bool isProtected() const { return _isProtected; }

    void setProtected(bool prot) { _isProtected = prot; }

    float getProgress() const { return _progress; }
protected:
    float _progress = 0.0f;
private:
    bool _isActive;
    bool _isProtected = false;
    EasingType _easing;
    uint32_t _startTime;
    uint32_t _duration;
};

class AnimationManager {
public:
    void addAnimation(std::shared_ptr<Animation> animation);
    void update(uint32_t currentTime);
    void clear();
    
    // 保护机制
    void markProtected(std::shared_ptr<Animation> animation);
    void clearUnprotected();
    void clearAllProtectionMarks();
    
    size_t activeCount() const;
private:
    etl::vector<std::shared_ptr<Animation>, MAX_ANIMATION_COUNT> _animations;
};

class CallbackAnimation : public Animation {
public:
    CallbackAnimation(float startVal, float endVal, uint32_t duration, EasingType easing,
                      std::function<void(float)> updateCallback)
        : Animation(duration, easing),
          _startVal(startVal),
          _endVal(endVal),
          _updateCallback(updateCallback) {}

    bool update(uint32_t currentTime) override {
        bool isRunning = Animation::update(currentTime);
        if (_updateCallback) {
            float currentValue = _startVal + (_endVal - _startVal) * _progress;
            _updateCallback(currentValue);
        }
        return isRunning;
    }

private:
    float _startVal;
    float _endVal;
    std::function<void(float)> _updateCallback;
};