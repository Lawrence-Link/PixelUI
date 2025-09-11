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
#include "IPopupRenderer.h"
#include <string>
#include <memory>
#include <functional>
#include <cstdint>
#include "PixelUI/core/CommonTypes.h"
// 前向声明
class AnimationManager;

class Popup {
public:
    Popup(IPopupRenderer& renderer, AnimationManager& animManager, 
          const std::string& message, PopupType type = PopupType::INFO);
    ~Popup() = default;

    void show(uint32_t duration = 0);
    void hide();
    void draw();
    void update(uint32_t currentTime);

    // 回调设置
    void setOnCloseCallback(std::function<void()> callback) { onCloseCallback_ = callback; }
    void setOnConfirmCallback(std::function<void(bool)> callback) { onConfirmCallback_ = callback; }

    // 状态查询
    bool isVisible() const { return isVisible_; }
    PopupType getType() const { return type_; }
    uint32_t getShowTime() const { return showTime_; }

    // 输入处理 - 之前遗漏的方法
    void handleConfirm(bool confirmed);
    void setSelfWeakPtr(std::weak_ptr<Popup> weakPtr) { selfWeakPtr_ = weakPtr; }
    
private:
    IPopupRenderer& renderer_;
    AnimationManager& animManager_;
    std::string message_;
    PopupType type_;
    bool isVisible_;
    uint32_t showTime_;
    uint32_t duration_;
    std::weak_ptr<Popup> selfWeakPtr_;

    float boxY_;
    float backgroundAlpha_;
    
    std::function<void()> onCloseCallback_;
    std::function<void(bool)> onConfirmCallback_;
    
    void animateIn();
    void animateOut();
    void drawBox();
    void drawMessage();
    void drawButtons();
};