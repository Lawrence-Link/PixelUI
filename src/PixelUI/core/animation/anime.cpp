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
        case EasingType::EASE_OUT_BOUNCE:   return easeOutBounce(t); // use the specified function to calculate
        default:                            return t; // default means linear
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
*   begin the animation
*/
void Animation::start(uint32_t currentTime) { // 启动动画显示
    _startTime = currentTime;
    _progress = 0;
    _isActive = true;
}
/*
*    stop the animation
*/

void Animation::stop() { // 终止动画
    _isActive = false;
}

/* 
* update the coordinates 更新动画
* @param currentTime Current timestamp
* @return {boolean} is the animation ongoing? 
*/
bool Animation::update(uint32_t currentTime){
    if (!_isActive) {
        return false; // animation not active, 动画非进行中
    }

    uint32_t elapsed = currentTime - _startTime; 
    if (elapsed >= _duration) {
        _progress = 1.0f;
        _isActive = false;
        return false; // the End of the animation, 动画完成
    }
// if (_onCompleteCallback) {
//             _onCompleteCallback();
//         }
    float t = static_cast<float> (elapsed) / static_cast<float>(_duration);
    _progress = EasingCalculator::calculate(_easing, t);
    return true; // animation is ongoing, 动画继续
}

void AnimationManager::addAnimation(std::shared_ptr<Animation> animation) {
    if (!animation) {
        std::cerr << "[ERROR] Trying to add null animation!" << std::endl;
        return;
    }
    
    std::cout << "[DEBUG] Adding animation to manager. Current count: " << _animations.size() << std::endl;
    _animations.push_back(animation);
    std::cout << "[DEBUG] Animation added. New count: " << _animations.size() << std::endl;
}

void AnimationManager::update(uint32_t currentTime) {
    // std::cout << "[DEBUG] AnimationManager::update() called with " << _animations.size() << " animations" << std::endl;
    
    if (_animations.empty()) {
        return;
    }
    
    try {
        // 安全的反向遍历删除
        for (auto it = _animations.rbegin(); it != _animations.rend();) {
            if (!*it) {
                std::cerr << "[ERROR] Null animation found in manager!" << std::endl;
                it = std::vector<std::shared_ptr<Animation>>::reverse_iterator(
                    _animations.erase((it + 1).base())
                );
                continue;
            }
            
            if (!(*it)->update(currentTime)) {
                std::cout << "[DEBUG] Animation completed, removing" << std::endl;
                it = std::vector<std::shared_ptr<Animation>>::reverse_iterator(
                    _animations.erase((it + 1).base())
                );
            } else {
                ++it;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in AnimationManager::update: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "[ERROR] Unknown exception in AnimationManager::update" << std::endl;
    }
    
    // std::cout << "[DEBUG] AnimationManager::update() finished with " << _animations.size() << " animations" << std::endl;
}

void AnimationManager::clear(){
    _animations.clear();
}

size_t AnimationManager::activeCount() const {
    return _animations.size();
}