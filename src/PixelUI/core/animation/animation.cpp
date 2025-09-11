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

#include "PixelUI/core/animation/animation.h"

// 浮点乘法转为整数乘法和位移
#define MUL_FIXED(a, b) ((int64_t)(a) * (b) >> SHIFT_BITS)

/**
 * @brief 缓动函数实现，所有计算均使用定点数。
 * @param type 缓动类型。
 * @param t 归一化的时间，定点数。
 * @return 归一化的进度，定点数。
 */
int32_t EasingCalculator::calculate(EasingType type, int32_t t) 
{
    switch (type) 
    {
        case EasingType::LINEAR:            return t;
        case EasingType::EASE_IN_QUAD:      return MUL_FIXED(t, t);
        case EasingType::EASE_OUT_QUAD:     
        {
            int32_t inverse_t = FIXED_POINT_ONE - t;
            return FIXED_POINT_ONE - MUL_FIXED(inverse_t, inverse_t);
        }
        case EasingType::EASE_IN_OUT_QUAD:  
            if (t < (FIXED_POINT_ONE / 2)) {
                return MUL_FIXED(2 * t, t);
            } else {
                int32_t val = (FIXED_POINT_ONE * 2) - (2 * t);
                return FIXED_POINT_ONE - MUL_FIXED(val, val) / 2;
            }
        case EasingType::EASE_IN_CUBIC:     return MUL_FIXED(MUL_FIXED(t, t), t);
        case EasingType::EASE_OUT_CUBIC:    
        {
            int32_t inverse_t = FIXED_POINT_ONE - t;
            return FIXED_POINT_ONE - MUL_FIXED(MUL_FIXED(inverse_t, inverse_t), inverse_t);
        }
        case EasingType::EASE_IN_OUT_CUBIC: 
            if (t < (FIXED_POINT_ONE / 2)) {
                return MUL_FIXED(4 * t, MUL_FIXED(t, t));
            } else {
                int32_t val = (FIXED_POINT_ONE * 2) - (2 * t);
                return FIXED_POINT_ONE - MUL_FIXED(MUL_FIXED(val, val), val) / 2;
            }
        case EasingType::EASE_OUT_BOUNCE:   return easeOutBounce(t);
        default:                            return t;
    }
}

/**
 * @brief EaseOutBounce 缓动函数，使用定点数实现。
 * @param t 归一化的时间，定点数。
 * @return 归一化的进度，定点数。
 */
int32_t EasingCalculator::easeOutBounce(int32_t t) {
    // 使用宏来简化定点数运算
    #define FIXED_DIV(a, b) ((int64_t)(a) * FIXED_POINT_ONE / (b))
    #define FIXED_MUL(a, b) ((int64_t)(a) * (b) / FIXED_POINT_ONE)

    const int32_t n1 = FLOAT_TO_FIXED(7.5625f);
    const int32_t d1 = FLOAT_TO_FIXED(2.75f);
    
    if (t < FIXED_DIV(FIXED_POINT_ONE, d1)) 
    {
        return FIXED_MUL(n1, FIXED_MUL(t, t));
    } 
    else if (t < FIXED_DIV(FLOAT_TO_FIXED(2.0f), d1))
    {
        t -= FIXED_DIV(FLOAT_TO_FIXED(1.5f), d1);
        return FIXED_MUL(n1, FIXED_MUL(t, t)) + FLOAT_TO_FIXED(0.75f);
    }
    else if (t < FIXED_DIV(FLOAT_TO_FIXED(2.5f), d1))
    {
        t -= FIXED_DIV(FLOAT_TO_FIXED(2.25f), d1);
        return FIXED_MUL(n1, FIXED_MUL(t, t)) + FLOAT_TO_FIXED(0.9375f);
    }
    else 
    {
        t -= FIXED_DIV(FLOAT_TO_FIXED(2.625f), d1);
        return FIXED_MUL(n1, FIXED_MUL(t, t)) + FLOAT_TO_FIXED(0.984375f);
    }
}

/*
@brief 启动动画，设置开始时间并标记为活动状态。
@param currentTime 当前时间（毫秒）。
*/
void Animation::start(uint32_t currentTime) {
    _startTime = currentTime;
    _progress = 0;
    _isActive = true;
}

/*
@brief 停止动画，标记为非活动状态。
*/
void Animation::stop() {
    _isActive = false;
}

/*
@brief 根据当前时间更新动画进度。
@param currentTime 当前时间（毫秒）。
@return 如果动画仍在运行则返回 true，否则返回 false。
*/
bool Animation::update(uint32_t currentTime){
    if (!_isActive) {
        return false;
    }

    uint32_t elapsed = currentTime - _startTime;
    
    // 如果动画已完成，将进度强制设置为 1.0 的定点数表示
    bool completed = (elapsed >= _duration);
    
    // 计算归一化的时间 't'，使用 64 位整型避免溢出
    int32_t t = completed ? FIXED_POINT_ONE : ((int64_t)elapsed * FIXED_POINT_ONE) / _duration;

    // 通过缓动函数计算最终进度
    _progress = EasingCalculator::calculate(_easing, t);

    if (completed) {
        _isActive = false;
        return false;
    }
    return true;
}

/*
@brief 向管理器添加一个新动画。
@param animation 指向要添加的 Animation 对象的智能指针。
*/
void AnimationManager::addAnimation(std::shared_ptr<Animation> animation) {
    if (!animation) {
        return;
    }
    _animations.push_back(animation);
}
/*
@brief 根据当前时间更新所有活动动画。
@param currentTime 当前时间（毫秒）。
*/
void AnimationManager::update(uint32_t currentTime) {
    if (_animations.empty()) {
        return;
    }

    auto writePos = _animations.begin();
    for (auto readPos = _animations.begin(); readPos != _animations.end(); ++readPos) {
        if ((*readPos)->update(currentTime)) {
            if (writePos != readPos) {
                *writePos = std::move(*readPos);
            }
            ++writePos;
        } else {
            // 如果动画已完成且未受保护，则将其移除
            if (!readPos->get()->isProtected()) {
                // 确保动画完成后取消保护，以便下次可以被清除
                readPos->get()->setProtected(false);
            }
        }
    }
    _animations.erase(writePos, _animations.end());
}

/*
@brief 清除管理器中的所有动画。
*/
void AnimationManager::clear(){
    _animations.clear();
}

/*
@brief 标记一个动画为受保护，使其在清理时不会被移除。
@param animation 指向要标记为受保护的 Animation 对象的智能指针。
*/
void AnimationManager::markProtected(std::shared_ptr<Animation> animation) {
    if (animation) {
        animation->setProtected(true);
    }
}
/*
@brief 清除管理器中所有未受保护的动画。
*/
void AnimationManager::clearUnprotected() {
    if (_animations.empty()) {
        return;
    }
    
    auto writePos = _animations.begin();
    for (auto readPos = _animations.begin(); readPos != _animations.end(); ++readPos) {
        if (readPos->get()->isProtected()) {
            if (writePos != readPos) {
                *writePos = std::move(*readPos);
            }
            ++writePos;
        }
    }
    _animations.erase(writePos, _animations.end());
}

/*
@brief 清除所有动画的保护标记。
*/
void AnimationManager::clearAllProtectionMarks() {
    for (const auto& anim_ : _animations) {
        anim_->setProtected(false);
    }
}

size_t AnimationManager::activeCount() const {
    return _animations.size();
}