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

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <cstdint>
#include "PixelUI/core/CommonTypes.h"

// 前向声明
class Popup;
class IPopupRenderer;
class AnimationManager;

// 输入类型枚举
enum class InputType {
    CONFIRM,
    CANCEL,
    ESCAPE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class PopupManager {
public:
    // 构造函数和析构函数
    PopupManager(IPopupRenderer& renderer, AnimationManager& animManager);
    ~PopupManager();

    // 基本显示方法
    void showInfo(const std::string& message, uint32_t duration = 2000);
    void showWarning(const std::string& message, uint32_t duration = 3000);
    void showError(const std::string& message, uint32_t duration = 4000);
    void showConfirm(const std::string& message, 
                    std::function<void(bool)> callback,
                    uint32_t duration = 0);

    // 核心更新和绘制方法
    void update(uint32_t currentTime);
    void draw();

    // 管理方法
    void hideAll();
    void clear();
    bool hasActivePopups() const;
    
    // 状态查询
    size_t getActivePopupCount() const;
    size_t getTotalPopupCount() const;
    
    // 配置方法
    void setMaxConcurrentPopups(size_t maxCount);
    size_t getMaxConcurrentPopups() const { return maxConcurrentPopups_; }

    // 输入处理
    void handleInput(InputType input);

    // 高级功能
    void showWithCustomAnimation(std::shared_ptr<Popup> popup, 
                                uint32_t duration = 2000,
                                EasingType easing = EasingType::EASE_OUT_QUAD);
    
    // 批量操作
    void showMultiple(const std::vector<std::pair<std::string, PopupType>>& messages,
                     uint32_t duration = 2000, 
                     uint32_t delayBetween = 500);

    // 禁用拷贝构造和赋值
    PopupManager(const PopupManager&) = delete;
    PopupManager& operator=(const PopupManager&) = delete;

    // 支持移动语义
    PopupManager(PopupManager&&) = default;
    PopupManager& operator=(PopupManager&&) = default;

    // 调试方法
    void printDebugInfo() const;
    size_t getTotalPopupsCreated() const { return totalPopupsCreated_; }
    size_t getTotalPopupsDestroyed() const { return totalPopupsDestroyed_; }

private:
    // 核心引用
    IPopupRenderer& renderer_;
    AnimationManager& animManager_;

    // Popup管理 - 这是我之前遗漏的关键成员
    std::vector<std::weak_ptr<Popup>> popups_;
    size_t maxConcurrentPopups_;

    // 调试和统计计数器 - 之前遗漏的成员
    mutable size_t totalPopupsCreated_;
    mutable size_t totalPopupsDestroyed_;

    // 内部辅助方法
    void addPopup(std::shared_ptr<Popup> popup, uint32_t duration);
    void removePopup(std::shared_ptr<Popup> popup);
    void cleanupExpiredPopups();
};

// 辅助结构体
struct PopupInfo {
    std::string message;
    PopupType type;
    uint32_t duration;
    std::function<void(bool)> callback;

    PopupInfo(const std::string& msg, PopupType t, uint32_t dur = 2000)
        : message(msg), type(t), duration(dur) {}
    
    PopupInfo(const std::string& msg, PopupType t, uint32_t dur, std::function<void(bool)> cb)
        : message(msg), type(t), duration(dur), callback(cb) {}
};

// 便利工厂方法
namespace PopupFactory {
    inline PopupInfo info(const std::string& message, uint32_t duration = 2000) {
        return PopupInfo(message, PopupType::INFO, duration);
    }
    
    inline PopupInfo warning(const std::string& message, uint32_t duration = 3000) {
        return PopupInfo(message, PopupType::WARNING, duration);
    }
    
    inline PopupInfo error(const std::string& message, uint32_t duration = 4000) {
        return PopupInfo(message, PopupType::ERROR, duration);
    }
    
    inline PopupInfo confirm(const std::string& message, std::function<void(bool)> callback) {
        return PopupInfo(message, PopupType::CONFIRM, 0, callback);
    }
}