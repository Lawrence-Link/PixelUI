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

#include "PixelUI/core/ui/Popup/PopupManager.h"
#include "PixelUI/core/ui/Popup/Popup.h"
#include "PixelUI/core/animation/animation.h"
#include "PixelUI/pixelui.h"
#include <algorithm>
#include <iostream>

PopupManager::PopupManager(IPopupRenderer& renderer, AnimationManager& animManager)
    : renderer_(renderer), animManager_(animManager), maxConcurrentPopups_(3) {
}

PopupManager::~PopupManager() {
    clear();
}

void PopupManager::showInfo(const std::string& message, uint32_t duration) {
    auto popup = std::make_shared<Popup>(renderer_, animManager_, message, PopupType::INFO);
    addPopup(popup, duration);
}

void PopupManager::showWarning(const std::string& message, uint32_t duration) {
    auto popup = std::make_shared<Popup>(renderer_, animManager_, message, PopupType::WARNING);
    addPopup(popup, duration);
}

void PopupManager::showError(const std::string& message, uint32_t duration) {
    auto popup = std::make_shared<Popup>(renderer_, animManager_, message, PopupType::ERROR);
    addPopup(popup, duration);
}

void PopupManager::showConfirm(const std::string& message, 
                              std::function<void(bool)> callback,
                              uint32_t duration) {
    auto popup = std::make_shared<Popup>(renderer_, animManager_, message, PopupType::CONFIRM);
    
    // 设置确认弹窗的回调处理
    popup->setOnCloseCallback([callback, popup]() {
        if (callback) {
            // 这里可以根据用户的选择传递true或false
            // 暂时默认为false（取消），实际实现需要处理用户输入
            callback(false);
        }
    });
    
    addPopup(popup, duration);
}

void PopupManager::addPopup(std::shared_ptr<Popup> popup, uint32_t duration) {
    if (!popup) return;
    
    // 设置weak_ptr避免循环引用
    popup->setSelfWeakPtr(popup);
    
    // 使用weak_ptr在回调中
    std::weak_ptr<Popup> weakPopup = popup;
    popup->setOnCloseCallback([this, weakPopup]() {
        auto strongPopup = weakPopup.lock();
        if (strongPopup) {
            removePopup(strongPopup);
        }
        renderer_.markDirty();
    });
    
    popups_.push_back(popup);
    popup->show(duration);
    renderer_.markDirty();
}

void PopupManager::removePopup(std::shared_ptr<Popup> popup) {
    if (!popup) return;
    
    auto it = std::find_if(popups_.begin(), popups_.end(),
        [popup](const std::weak_ptr<Popup>& weakPtr) {
            auto ptr = weakPtr.lock();
            return ptr && ptr == popup;
        });
    
    if (it != popups_.end()) {
        popups_.erase(it);
        std::cout << "[DEBUG] PopupManager: Removed popup, remaining count: " << popups_.size() << std::endl;
    }
}

void PopupManager::hideAll() {
    for (auto& weakPopup : popups_) {
        auto popup = weakPopup.lock();
        if (popup) {
            popup->hide();
        }
    }
    
    // 清理失效的弱引用
    cleanupExpiredPopups();
}

void PopupManager::clear() {
    // 先隐藏所有popup
    for (auto& weakPopup : popups_) {
        auto popup = weakPopup.lock();
        if (popup) {
            popup->hide();
        }
    }
    
    // 清空列表
    popups_.clear();
    
    // 标记需要重绘
    renderer_.markDirty();
    
    std::cout << "[DEBUG] PopupManager: Cleared all popups" << std::endl;
}

void PopupManager::update(uint32_t currentTime) {
    // 更新所有存活的popup
    for (auto& weakPopup : popups_) {
        auto popup = weakPopup.lock();
        if (popup) {
            popup->update(currentTime);
        }
    }
    
    // 定期清理失效的弱引用
    static uint32_t lastCleanupTime = 0;
    if (currentTime - lastCleanupTime > 1000) {  // 每秒清理一次
        cleanupExpiredPopups();
        lastCleanupTime = currentTime;
    }
}

void PopupManager::draw() {
    // 按显示时间排序绘制，确保新的popup在上层
    std::vector<std::shared_ptr<Popup>> activePopups;
    
    for (auto& weakPopup : popups_) {
        auto popup = weakPopup.lock();
        if (popup && popup->isVisible()) {
            activePopups.push_back(popup);
        }
    }
    
    // 按显示时间排序，早显示的先绘制（在底层）
    std::sort(activePopups.begin(), activePopups.end(),
        [](const std::shared_ptr<Popup>& a, const std::shared_ptr<Popup>& b) {
            return a->getShowTime() < b->getShowTime();
        });
    
    // 绘制所有可见的popup
    for (auto& popup : activePopups) {
        popup->draw();
    }
}

void PopupManager::cleanupExpiredPopups() {
    auto it = std::remove_if(popups_.begin(), popups_.end(),
        [](const std::weak_ptr<Popup>& weakPtr) {
            return weakPtr.expired();
        });
    
    if (it != popups_.end()) {
        size_t removedCount = std::distance(it, popups_.end());
        popups_.erase(it, popups_.end());
        std::cout << "[DEBUG] PopupManager: Cleaned up " << removedCount << " expired popups" << std::endl;
    }
}

void PopupManager::setMaxConcurrentPopups(size_t maxCount) {
    maxConcurrentPopups_ = maxCount;
    
    // 如果当前popup数量超过新的限制，移除多余的
    while (popups_.size() > maxConcurrentPopups_) {
        auto oldestIt = std::min_element(popups_.begin(), popups_.end(),
            [](const std::weak_ptr<Popup>& a, const std::weak_ptr<Popup>& b) {
                auto aPtr = a.lock();
                auto bPtr = b.lock();
                if (!aPtr) return true;
                if (!bPtr) return false;
                return aPtr->getShowTime() < bPtr->getShowTime();
            });
        
        if (oldestIt != popups_.end()) {
            auto oldestPopup = oldestIt->lock();
            if (oldestPopup) {
                oldestPopup->hide();
            }
            popups_.erase(oldestIt);
        }
    }
}

size_t PopupManager::getActivePopupCount() const {
    size_t count = 0;
    for (const auto& weakPopup : popups_) {
        auto popup = weakPopup.lock();
        if (popup && popup->isVisible()) {
            count++;
        }
    }
    return count;
}

size_t PopupManager::getTotalPopupCount() const {
    return popups_.size();
}

bool PopupManager::hasActivePopups() const {
    for (const auto& weakPopup : popups_) {
        auto popup = weakPopup.lock();
        if (popup && popup->isVisible()) {
            return true;
        }
    }
    return false;
}

void PopupManager::handleInput(InputType input) {
    // 处理最顶层（最新）的可见popup的输入
    std::shared_ptr<Popup> topPopup = nullptr;
    uint32_t latestShowTime = 0;
    
    for (auto& weakPopup : popups_) {
        auto popup = weakPopup.lock();
        if (popup && popup->isVisible()) {
            if (popup->getShowTime() > latestShowTime) {
                latestShowTime = popup->getShowTime();
                topPopup = popup;
            }
        }
    }
    
    if (topPopup) {
        switch (input) {
            case InputType::CONFIRM:
                if (topPopup->getType() == PopupType::CONFIRM) {
                    // 处理确认操作
                    topPopup->handleConfirm(true);
                } else {
                    // 其他类型的popup，确认键直接关闭
                    topPopup->hide();
                }
                break;
                
            case InputType::CANCEL:
                if (topPopup->getType() == PopupType::CONFIRM) {
                    // 处理取消操作
                    topPopup->handleConfirm(false);
                } else {
                    // 其他类型的popup，取消键直接关闭
                    topPopup->hide();
                }
                break;
                
            case InputType::ESCAPE:
                // ESC键总是关闭最顶层的popup
                topPopup->hide();
                break;
                
            default:
                // 其他输入类型暂不处理
                break;
        }
        
        renderer_.markDirty();
    }
}

// 辅助方法：创建带淡入动画的popup显示效果
void PopupManager::showWithCustomAnimation(std::shared_ptr<Popup> popup, 
                                          uint32_t duration,
                                          EasingType easing) {
    if (!popup) return;
    
    // 设置自定义动画缓动类型
    // 这可以通过修改Popup类来支持自定义动画参数
    addPopup(popup, duration);
}

// 批量操作方法
void PopupManager::showMultiple(const std::vector<std::pair<std::string, PopupType>>& messages,
                               uint32_t duration, uint32_t delayBetween) {
    uint32_t currentDelay = 0;
    
    for (const auto& [message, type] : messages) {
        // 使用动画系统创建延迟显示效果
        auto delayAnim = std::make_shared<CallbackAnimation>(
            0.0f, 1.0f, currentDelay, EasingType::LINEAR,
            [this, message, type, duration](float progress) {
                if (progress >= 1.0f) {
                    switch (type) {
                        case PopupType::INFO:
                            showInfo(message, duration);
                            break;
                        case PopupType::WARNING:
                            showWarning(message, duration);
                            break;
                        case PopupType::ERROR:
                            showError(message, duration);
                            break;
                        default:
                            showInfo(message, duration);
                            break;
                    }
                }
            }
        );
        
        renderer_.addAnimation(delayAnim);
        currentDelay += delayBetween;
    }
}