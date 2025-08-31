#include "PixelUI/core/animation/anime.h"

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

void Animation::start(uint32_t currentTime) {
    _startTime = currentTime;
    _progress = 0;
    _isActive = true;
}

void Animation::stop() {
    _isActive = false;
}

bool Animation::update(uint32_t currentTime){
    if (!_isActive) {
        return false;
    }

    uint32_t elapsed = currentTime - _startTime; 
    if (elapsed >= _duration) {
        _progress = 1.0f;
        _isActive = false;
        return false;
    }

    float t = static_cast<float> (elapsed) / static_cast<float>(_duration);
    _progress = EasingCalculator::calculate(_easing, t);
    return true;
}

void AnimationManager::addAnimation(std::shared_ptr<Animation> animation) {
    if (!animation) {
        std::cerr << "[ERROR] Trying to add null animation!" << std::endl;
        return;
    }
    #ifdef DEBUG
    std::cout << "[DEBUG] Adding animation to manager. Current count: " << _animations.size() << std::endl;
    #endif
    _animations.push_back(animation);
    #ifdef DEBUG
    std::cout << "[DEBUG] Animation added. New count: " << _animations.size() << std::endl;
    #endif
}

void AnimationManager::update(uint32_t currentTime) {
    if (_animations.empty()) {
        return;
    }
    
    try {
        auto writePos = _animations.begin();
        for (auto readPos = _animations.begin(); readPos != _animations.end(); ++readPos) {
            if (!*readPos) {
                std::cerr << "[ERROR] Null animation found in manager!" << std::endl;
                continue;
            }
            
            if ((*readPos)->update(currentTime)) {
                if (writePos != readPos) {
                    *writePos = std::move(*readPos);
                }
                ++writePos;
            } else { // deploy delete algorithm here.
                #ifdef DEBUG
                std::cout << "[DEBUG] Animation completed, removing" << std::endl;
                #endif
                // auto protectedIt = std::find(_protectedAnimations.begin(), _protectedAnimations.end(), *readPos);
                // if (protectedIt != _protectedAnimations.end()) { = false
                //     _protectedAnimations.erase(protectedIt);
                // }
                if (!(readPos->get()->isProtected())) {
                    readPos->get()->setProtected(false); // remove from protection after it was done :()
                }
            }
        }
        
        _animations.erase(writePos, _animations.end());
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in AnimationManager::update: " << e.what() << std::endl;
    }
}

void AnimationManager::clear(){
    _animations.clear(); // reset all
}

void AnimationManager::markProtected(std::shared_ptr<Animation> animation) {
    if (animation) {
        animation->setProtected(true);
        #ifdef DEBUG
        std::cout << "[DEBUG] Marked animation as protected. Protected count: " << _protectedAnimations.size() << std::endl;
        #endif
    }
}

void AnimationManager::clearUnprotected() {
    if (_animations.empty()) {
        return;
    }
    
    auto writePos = _animations.begin();
    for (auto readPos = _animations.begin(); readPos != _animations.end(); ++readPos) {
        bool isProtected = false;
        
        if (readPos ->get()->isProtected()) {
            if (writePos != readPos) {
                *writePos = std::move(*readPos);
            }
            ++writePos;
        } else {
            #ifdef DEBUG
            std::cout << "[DEBUG] Clearing unprotected animation" << std::endl;
            #endif
        }
    }
    
    _animations.erase(writePos, _animations.end());
}

void AnimationManager::clearAllProtectionMarks() {
    for (const auto& anim_ : _animations) {
        anim_->setProtected(false);
    }
}

size_t AnimationManager::activeCount() const {
    return _animations.size();
}