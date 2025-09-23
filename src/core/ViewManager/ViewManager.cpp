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

#include "core/ViewManager/ViewManager.h"
/*
@brief Pushes a new application onto the view stack and makes it the current view.
@param app A shared pointer to the application to be pushed onto the stack.
*/
void ViewManager::push(std::shared_ptr<IApplication> app) {
    if (!app) return;
        std::lock_guard<std::mutex> lock(m_stackMutex);
        m_isTransitioning = true;

        if (!m_viewStack.empty()) {
            m_viewStack.top()->onPause(); // Pause the current top application
        }

        m_viewStack.push(app);    // Push the new application onto the stack
        m_ui.setDrawable(app);    // Grant app with drawable control
        
        app->onEnter([this]() {this->pop();}); // Handle app with exit callback
        m_ui.markDirty();

        m_isTransitioning = false;
}

/*
@brief Pops the current application from the view stack and resumes the previous application if available.
*/
void ViewManager::pop() {
    std::lock_guard<std::mutex> lock(m_stackMutex);

    if (m_viewStack.empty()) return;
    
    m_isTransitioning = true; // 标志转换开始

    auto appToExit = m_viewStack.top();

    // 关键改动：立即清除 drawable 指针，避免渲染器访问即将销毁的对象
    m_ui.setDrawable(nullptr); 
    
    appToExit->onExit(); // 现在可以安全地调用 onExit
    m_viewStack.pop(); 

    if (!m_viewStack.empty()) {
        auto& previousApp = m_viewStack.top();
        m_ui.setDrawable( previousApp ); // 设置新的 drawable
        previousApp->onResume(); // resume the previous application
    }
    // else 分支的 m_ui.setDrawable(nullptr) 已经提前执行了，无需重复
    
    m_ui.markDirty();
    m_isTransitioning = false; // 标志转换结束
}

std::shared_ptr<IApplication> ViewManager::getCurrentApp() const {
    std::lock_guard<std::mutex> lock(m_stackMutex);
    if (m_viewStack.empty()) return nullptr;
    return m_viewStack.top();
}