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
#include "PixelUI/core/animation/animation.h" // 确保包含 animation.h 以访问 CallbackAnimation

PixelUI::PixelUI(U8G2& u8g2) : u8g2_(u8g2), _currentTime(0) {
    m_viewManagerPtr = std::make_shared<ViewManager>(*this);
    m_animationManagerPtr = std::make_shared<AnimationManager>();
}

void PixelUI::begin() {
    AppManager::getInstance().sortByOrder();
}

void PixelUI::Heartbeat(uint32_t ms) 
{
    _currentTime += ms;
    
    m_animationManagerPtr->update(_currentTime);
}

void PixelUI::addAnimation(std::shared_ptr<Animation> animation) {
    animation->start(_currentTime);
    m_animationManagerPtr->addAnimation(animation);
}

/**
 * @brief 创建并启动一个单值动画。
 * @param value 引用要进行动画的整数值（定点数）。
 * @param targetValue 目标整数值（定点数）。
 * @param duration 动画持续时间（毫秒）。
 * @param easing 缓动类型。
 * @param prot 动画保护状态。
 */
void PixelUI::animate(int32_t& value, int32_t targetValue, uint32_t duration, EasingType easing, PROTECTION prot) {
    auto animation = std::make_shared<CallbackAnimation>(
        value, targetValue, duration, easing,
        [&value](int32_t currentValue) {
            value = currentValue;
        }
    );
    if (prot == PROTECTION::PROTECTED) m_animationManagerPtr->markProtected(animation);
    addAnimation(animation);
}

/**
 * @brief 创建并启动一个二维坐标动画。
 * @param x 引用要进行动画的 x 坐标（定点数）。
 * @param y 引用要进行动画的 y 坐标（定点数）。
 * @param targetX 目标 x 坐标（定点数）。
 * @param targetY 目标 y 坐标（定点数）。
 * @param duration 动画持续时间（毫秒）。
 * @param easing 缓动类型。
 * @param prot 动画保护状态。
 */
void PixelUI::animate(int32_t& x, int32_t& y, int32_t targetX, int32_t targetY, uint32_t duration, EasingType easing, PROTECTION prot) {
    // 为 X 坐标创建动画
    auto animX = std::make_shared<CallbackAnimation>(
        x, targetX, duration, easing,
        [&x](int32_t currentValue) {
            x = currentValue;
        }
    );
    addAnimation(animX);

    // 为 Y 坐标创建动画
    auto animY = std::make_shared<CallbackAnimation>(
        y, targetY, duration, easing,
        [&y](int32_t currentValue) {
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
        
        this->getU8G2().sendBuffer();
    } else {
        uint8_t * buf_ptr = this->getU8G2().getBufferPtr();
        uint16_t buf_len = 1024;
        for (int fade = 1; fade <= 4; fade++){
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