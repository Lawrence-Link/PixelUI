/*
 * Copyright (C) 2025 Lawrence Link
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

#include "core/app/IApplication.h"
#include <stack>
#include <memory>
#include <mutex>
#include <atomic>
#include "ui/Popup/Popup.h"

class ViewManager {
public:
    ViewManager(PixelUI &ui) : m_ui(ui) {
        m_ui.setInputCallback ([&](InputEvent event) -> bool {
            // 优先处理弹窗输入 - 检查是否有活动的popup
            auto popupManager = m_ui.getPopupManagerPtr();
            if (popupManager && popupManager->getPopupCounts() > 0) {
                // 获取最高优先级的popup处理输入
                return popupManager->handleTopPopupInput(event);
            }
            
            // 弹窗没有处理或没有弹窗时,将输入传递给栈顶应用
            if (!m_viewStack.empty()) {
                return m_viewStack.top()->handleInput(event);
            }
            return false;
        });
    }
    void push(std::shared_ptr<IApplication> app);
    void pop();
    bool isTransitioning() const noexcept { return m_isTransitioning.load(std::memory_order_relaxed); }

    std::shared_ptr<IApplication> getCurrentApp() const;
private:
    PixelUI &m_ui;
    std::stack<std::shared_ptr<IApplication>> m_viewStack;
    mutable std::mutex m_stackMutex;
    std::atomic<bool> m_isTransitioning{false};
};