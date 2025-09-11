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

/*
@brief Easing functions implementation
@param type EasingType enum value
@param t Normalized time (0.0 to 1.0)
*/
float EasingCalculator::calculate(EasingType type, float t) 
{
    switch (type) 
    {
        case EasingType::LINEAR:            return t;
        case EasingType::EASE_IN_QUAD:      return t * t;
        case EasingType::EASE_OUT_QUAD:     return 1 - (1 - t) * (1 - t);
        case EasingType::EASE_IN_OUT_QUAD:  return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
        case EasingType::EASE_IN_CUBIC:     return t * t * t;
        case EasingType::EASE_OUT_CUBIC:    return 1 - pow(1 - t, 3);
        case EasingType::EASE_IN_OUT_CUBIC: return t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
        case EasingType::EASE_OUT_BOUNCE:   return easeOutBounce(t);
        default:                            return t;
    }
}

float EasingCalculator::easeOutBounce(float t) {
    const float n1 = 7.5625;
    const float d1 = 2.75;
    if (t < 1/d1) 
    {
        return n1 * t * t;
    } 
    else if (t < 2 / d1)
    {
        t -= 1.5/d1;
        return n1 * t * t + 0.75;
    }
    else if (t < 2.5 / d1)
    {
        t -= 2.25/d1;
        return n1 * t * t + 0.9375;
    }
    else 
    {
        t -= 2.625 / d1;
        return n1 * t * t + 0.984375;
    }
}

/*
@brief Starts the animation by setting the start time and marking it as active.
@param currentTime The current time in milliseconds.
*/
void Animation::start(uint32_t currentTime) {
    _startTime = currentTime;
    _progress = 0;
    _isActive = true;
}
/*
@brief Stops the animation and marks it as inactive.
*/
void Animation::stop() {
    _isActive = false;
}
/*
@brief Updates the animation progress based on the current time.
@param currentTime The current time in milliseconds.
*/
bool Animation::update(uint32_t currentTime){
    if (!_isActive) {
        return false;
    }

    uint32_t elapsed = currentTime - _startTime; 
    if (_duration > 250) {
        if (elapsed >= _duration - 50) { // eliminate overshoot when duration is long enough
            _progress = 1.0f;
            _isActive = false;
            return false;
        }
    } else {
        if (elapsed >= _duration) {
            _progress = 1.0f;
            _isActive = false;
            return false;
        }
    }
    float t = static_cast<float> (elapsed) / static_cast<float>(_duration);
    _progress = EasingCalculator::calculate(_easing, t);
    return true;
}

// bool Animation::update(uint32_t currentTime){
//     if (!_isActive) {
//         return false;
//     }

//     uint32_t elapsed = currentTime - _startTime; 
    
//     // 检查动画是否完成
//     bool completed = (elapsed >= _duration);

//     // 计算t值。如果已完成，确保t为1.0
//     float t = completed ? 1.0f : static_cast<float>(elapsed) / static_cast<float>(_duration);

//     // 始终通过缓动函数计算进度，确保终点平滑
//     _progress = EasingCalculator::calculate(_easing, t);

//     // 如果动画已完成，更新状态并返回
//     if (completed) {
//         _isActive = false;
//         return false;
//     }
//     return true;
// }

/*
@brief Adds a new animation to the manager.
@param animation Shared pointer to the Animation object to be added.
*/
void AnimationManager::addAnimation(std::shared_ptr<Animation> animation) {
    if (!animation) {
        return;
    }
    _animations.push_back(animation);
}
/*
@brief Updates all active animations based on the current time.
@param currentTime The current time in milliseconds.
*/
void AnimationManager::update(uint32_t currentTime) {
    if (_animations.empty()) {
        return;
    }
    // if (m_viewManager.isTransitioning()) return;

    // using the "erase-remove" idiom to remove finished animations
        auto writePos = _animations.begin();
        for (auto readPos = _animations.begin(); readPos != _animations.end(); ++readPos) {
            if ((*readPos)->update(currentTime)) {
                if (writePos != readPos) {
                    *writePos = std::move(*readPos);
                }
                ++writePos;
            } else { // deploy delete algorithm here.
                if (!(readPos->get()->isProtected())) {
                    readPos->get()->setProtected(false); // remove from protection after it was done :()
                }
            }
        }
        
        _animations.erase(writePos, _animations.end());
}

/*
@brief Clears all animations from the manager.
*/
void AnimationManager::clear(){
    _animations.clear(); // reset all
}

/*
@brief Marks an animation as protected, preventing it from being removed during cleanup.
@param animation Shared pointer to the Animation object to be marked as protected.
*/
void AnimationManager::markProtected(std::shared_ptr<Animation> animation) {
    if (animation) {
        animation->setProtected(true);
    }
}
/*
@brief Clears all unprotected animations from the manager.
*/
void AnimationManager::clearUnprotected() {
    if (_animations.empty()) {
        return;
    }
    // still the erase-remove idiom
    auto writePos = _animations.begin();
    for (auto readPos = _animations.begin(); readPos != _animations.end(); ++readPos) {
        bool isProtected = false;
        
        if (readPos ->get()->isProtected()) {
            if (writePos != readPos) {
                *writePos = std::move(*readPos);
            }
            ++writePos;
        }
    }
    
    _animations.erase(writePos, _animations.end());
}

/*
@brief Clears all protection marks from animations, allowing them to be removed during cleanup.
*/
void AnimationManager::clearAllProtectionMarks() {
    for (const auto& anim_ : _animations) {
        anim_->setProtected(false);
    }
}

size_t AnimationManager::activeCount() const {
    return _animations.size();
}