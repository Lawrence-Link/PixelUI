/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/ViewManager/ViewManager.h"
/*
@brief Pushes a new application onto the view stack and makes it the current view.
@param app A shared pointer to the application to be pushed onto the stack.
*/
void ViewManager::push(std::shared_ptr<IApplication> app) {
    if (!app) return;
        m_isTransitioning = true;

        if (!m_viewStack.empty()) {
            m_ui.clearAllCoroutines();
            m_ui.clearAllAnimations();
            m_viewStack.top()->onPause(); // Pause the current top application
        }

        m_viewStack.push(app);    // Push the new application onto the stack
        m_ui.setDrawable(app);    // Grant app with drawable control
        
        m_ui.clearFocusManager();
        app->onEnter([this]() {this->pop();}); // Handle app with exit callback
        m_ui.markDirty();

        m_isTransitioning = false;
}

/*
@brief Pops the current application from the view stack and resumes the previous application if available.
*/
void ViewManager::pop() {
    // std::lock_guard<std::mutex> lock(m_stackMutex);

    if (m_viewStack.empty()) return;

    m_isTransitioning = true;
    m_ui.setDrawable(nullptr); 
    
    m_viewStack.top()->onExit();  

    m_ui.markFading();

    m_viewStack.pop(); 

    m_ui.clearFocusManager();
    m_ui.clearAllCoroutines();
    m_ui.clearAllAnimations();
    if (!m_viewStack.empty()) {
        auto& previousApp = m_viewStack.top();
        m_ui.setDrawable( previousApp ); // setting up new drawable
        previousApp->onResume(); // resume the previous application
    }
    
    m_ui.markDirty();
    m_isTransitioning = false; // mark the end of the transition
}

std::shared_ptr<IApplication> ViewManager::getCurrentApp() const {
    if (m_viewStack.empty()) return nullptr;
    return m_viewStack.top();
}