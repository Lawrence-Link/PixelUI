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

#include "PixelUI/pixelui.h"
#include "PixelUI/core/ViewManager/ViewManager.h"
#include <cassert>
#include "EmuWorker.h"
#include <functional>
#include "PixelUI/core/app/app_system.h"

PixelUI::PixelUI(U8G2& u8g2) : u8g2_(u8g2), _currentTime(0) {
    // popupManager_ = std::make_unique<PopupManager>(*this, _animationManager); // TBD

    // To prevent from circular dependency, we use shared_ptr for both ViewManager and AnimationManager
    
    m_viewManagerPtr = std::make_shared<ViewManager>(*this);
    m_animationManagerPtr = std::make_shared<AnimationManager>();
    // sort app by order
    
}

void PixelUI::begin() {
    AppManager::getInstance().sortByOrder(); // sort app after registration
}

void PixelUI::Heartbeat(uint32_t ms) 
{
    _currentTime += ms;
    
    size_t beforeCount = m_animationManagerPtr->activeCount();
    m_animationManagerPtr->update(_currentTime);
    size_t afterCount = m_animationManagerPtr->activeCount();
    
    // 更新popup
    // if (popupManager_) {
    //     popupManager_->update(_currentTime);
    // }
}

void PixelUI::addAnimation(std::shared_ptr<Animation> animation) {
    animation->start(_currentTime); // begin the animation
    m_animationManagerPtr->addAnimation(animation); // add to manager
}

void PixelUI::animate(float& value, float targetValue, uint32_t duration, EasingType easing, PROTECTION prot) {
    auto animation = std::make_shared<CallbackAnimation>(
        value, targetValue, duration, easing,
        [&value](float currentValue) {
            value = currentValue;
        }
    );
    if (prot == PROTECTION::PROTECTED) m_animationManagerPtr->markProtected(animation);
    addAnimation(animation);
}

void PixelUI::animate(float& x, float& y, float targetX, float targetY, uint32_t duration, EasingType easing, PROTECTION prot) {
    // Create animation for both X and Y coordinates
    auto animX = std::make_shared<CallbackAnimation>(
        x, targetX, duration, easing,
        [&x](float currentValue) {
            x = currentValue;
        }
    );
    addAnimation(animX);

    auto animY = std::make_shared<CallbackAnimation>(
        y, targetY, duration, easing,
        [&y](float currentValue) {
            y = currentValue;
        }
    );
    addAnimation(animY);

    if (prot == PROTECTION::PROTECTED) {
        m_animationManagerPtr->markProtected(animX);
        m_animationManagerPtr->markProtected(animY);
    }
}

void PixelUI::renderer() {
    if (!isFading_){
        this->getU8G2().clearBuffer();
        
        if (currentDrawable_ && isDirty()) {
            currentDrawable_->draw();
            isDirty_ = false;
        }
        
        // 绘制popup
        // if (popupManager_) {
        //     popupManager_->draw();
        // }
        
        this->getU8G2().sendBuffer();
    } else { // isFading
        uint8_t * buf_ptr = this->getU8G2().getBufferPtr();
        uint16_t buf_len = 1024;
        for (int fade = 1; fade <= 4; fade++){
            // different fade stages, directly manipulate the buffer
            switch (fade)
            {
                case 1: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0xAA; break;
                case 2: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
                case 3: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x55; break;
                case 4: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
            }
            this->getU8G2().sendBuffer();
            if (m_refresh_callback) m_refresh_callback();
            m_func_delay(40);
        }
        isFading_ = false;
    }
}