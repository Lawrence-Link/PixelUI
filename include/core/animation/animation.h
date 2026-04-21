/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <cstdint>
#include <memory>
#include "etl/vector.h"
#include <functional>
#include "config.h"
#include "core/CommonTypes.h"

// fixed-point shift bits to support fractional values in integer arithmetic
// eg. SHIFT_BITS = 12 means 0.5f is represented as 0.5 * (1 << 12) = 2048
#define SHIFT_BITS 12
#define FIXED_POINT_ONE (1 << SHIFT_BITS)
#define FLOAT_TO_FIXED(f) ((int32_t)((f) * FIXED_POINT_ONE))

/**
 * @class EasingCalculator
 * @brief Calculate easing progress based on easing type and normalized time.
 */
class EasingCalculator {
public:
    /**
     * @brief calculate progress based on easing type and normalized time.
     * @param type easing type.
     * @param t normalized time, fixed-point.
     */
    static int32_t calculate(EasingType type, int32_t t);
private:
    /**
     * @brief ease out bounce function.
     * @param t normalized time, fixed-point.
     */
    static int32_t easeOutBounce(int32_t t);
};

/**
 * @class Animation
 * @brief Base class for animations, managing timing and easing.
 */
class Animation {
public:
    Animation(uint32_t duration, EasingType easing = EasingType::LINEAR) :
        _progress(0),           
        _isActive(false),       
        _isProtected(false),    
        _easing(easing),        
        _startTime(0),          
        _duration(duration)     
    {}
    
    virtual ~Animation() = default;
    
    void start(uint32_t currentTime);
    void stop();
    virtual bool update(uint32_t currentTime);
    bool isActive() const { return _isActive; }
    bool isProtected() const { return _isProtected; }
    void setProtected(bool prot) { _isProtected = prot; }
    int32_t getProgress() const { return _progress; }

protected:
    int32_t _progress = 0;     

private:
    bool _isActive;             
    bool _isProtected = false;  
    EasingType _easing;         
    uint32_t _startTime;        
    uint32_t _duration;         
};

/**
 * @class AnimationManager
 * @brief Manages multiple animations, updating and cleaning them up.
 */
class AnimationManager {
public:
    ~AnimationManager() = default;
    void addAnimation(std::shared_ptr<Animation> animation);
    void update(uint32_t currentTime);
    void clear();
    
    // Protection mechanism
    void markProtected(std::shared_ptr<Animation> animation);
    void clearUnprotected();
    void clearAllProtectionMarks();
    size_t activeCount() const;

private:
    etl::vector<std::shared_ptr<Animation>, MAX_ANIMATION_COUNT> _animations;
};

/**
 * @class CallbackAnimation
 * @brief Animation that calls a callback with the current value on each update.
 */
class CallbackAnimation : public Animation {
public:
    CallbackAnimation(int32_t startVal, int32_t endVal, uint32_t duration, EasingType easing,
                      std::function<void(int32_t)> updateCallback)
        : Animation(duration, easing),
          _startVal(startVal),
          _endVal(endVal),
          _updateCallback(updateCallback) {}
          
    bool update(uint32_t currentTime) override {
        bool isRunning = Animation::update(currentTime);
        if (_updateCallback) { 
            int32_t delta = _endVal - _startVal;
            int32_t currentValue = _startVal + ((int64_t)delta * _progress) / FIXED_POINT_ONE;
            _updateCallback(currentValue);
        }
        return isRunning;
    }

private:
    int32_t _startVal;
    int32_t _endVal;
    std::function<void(int32_t)> _updateCallback;
};