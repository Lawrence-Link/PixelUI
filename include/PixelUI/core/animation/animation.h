/*
 * Copyright (C) 2025 Lawrence Li
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <memory>
#include "etl/vector.h"
#include <functional>
#include "PixelUI/config.h"
#include "PixelUI/core/CommonTypes.h"
#include "PixelUI/core/animation/animation.h"

// 定义定点数位移值，用于将浮点数映射到整数。
// 例如，SHIFT_BITS = 12 时，0.5f 会被表示为 0.5 * (1 << 12) = 2048。
#define SHIFT_BITS 12
#define FIXED_POINT_ONE (1 << SHIFT_BITS)
#define FLOAT_TO_FIXED(f) ((int32_t)((f) * FIXED_POINT_ONE))

/**
 * @class EasingCalculator
 * @brief 负责计算动画的缓动曲线进度，所有计算均使用定点数（整型）。
 */
class EasingCalculator {
public:
    /**
     * @brief 根据缓动类型计算动画进度。
     * @param type 缓动类型，例如 EasingType::LINEAR。
     * @param t 归一化的时间，表示为定点数，范围为 [0, FIXED_POINT_ONE]。
     * @return 归一化的进度，表示为定点数，范围为 [0, FIXED_POINT_ONE]。
     */
    static int32_t calculate(EasingType type, int32_t t);

private:
    /**
     * @brief 实现 EaseOutBounce 缓动函数，使用定点数计算。
     * @param t 归一化的时间，定点数。
     * @return 归一化的进度，定点数。
     */
    static int32_t easeOutBounce(int32_t t);
};

/**
 * @class Animation
 * @brief 动画基类，管理动画的生命周期和进度。
 *
 * 将 _progress 从浮点数 float 修改为 int32_t，以支持定点数。
 */
class Animation {
public:
    Animation(uint32_t duration, EasingType easing = EasingType::LINEAR) :
        _duration(duration),
        _easing(easing), _startTime(0), _isActive(false), _progress(0) {};

    virtual ~Animation() = default;

    void start(uint32_t currentTime);
    void stop();
    virtual bool update(uint32_t currentTime);

    bool isActive() const { return _isActive; }
    bool isProtected() const { return _isProtected; }
    void setProtected(bool prot) { _isProtected = prot; }

    int32_t getProgress() const { return _progress; }
protected:
    int32_t _progress = 0; // 修改为定点数，表示范围 [0, FIXED_POINT_ONE]
private:
    bool _isActive;
    bool _isProtected = false;
    EasingType _easing;
    uint32_t _startTime;
    uint32_t _duration;
};

/**
 * @class AnimationManager
 * @brief 管理所有活动动画的生命周期。
 */
class AnimationManager {
public:
    ~AnimationManager() = default;
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

/**
 * @class CallbackAnimation
 * @brief 实现了通过回调函数更新数值的动画。
 *
 * 将 startVal 和 endVal 从 float 修改为 int32_t，以支持定点数。
 * 回调函数也修改为接受 int32_t 参数。
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
            // 使用整数运算计算当前值
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