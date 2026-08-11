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

#pragma once

#include "core/app/app_system.h"
#include <etl/stack.h>
#include <etl/atomic.h>
#include <etl/utility.h>
#include "ui/Popup/PopupManager.h"

class ViewManager {
public:
    enum class LaunchResult {
        Ok,
        StackFull,
        PoolFull,
        ConstructionFailed,
    };

    ViewManager(PixelUI &ui) : m_ui(ui) {
        m_ui.setInputCallback ([this](InputEvent event) -> bool {
            // Prioritize pop-up input - check for an active pop-up

            auto popupManager = m_ui.getPopupManagerPtr();
            if (popupManager->getPopupCounts() > 0) {
                // Get the highest priority pop-up to handle input
                return popupManager->handleTopPopupInput(event);
            }
            
            // If the pop-up did not handle the input or there is no pop-up, pass the input to the application at the top of the stack
            if (!m_viewStack.empty()) {
                return m_viewStack.top()->handleInput(event);
            }
            return false;
        });
    }
    LaunchResult push(ApplicationPtr app);
    LaunchResult launch(const AppItem& item, void* parameters = nullptr);

    template <typename T, typename... Args>
    ApplicationPtr makeApplication(Args&&... args) {
        return m_applicationPool.make<T>(etl::forward<Args>(args)...);
    }

    void pop();
    bool isTransitioning() const noexcept { return m_isTransitioning.load(etl::memory_order_relaxed); }

    IApplication* getCurrentApp() const;
private:
    PixelUI &m_ui;
    // The pool must be declared before the stack so it is destroyed after every handle.
    ApplicationPool m_applicationPool;
    etl::stack<ApplicationPtr, MAX_VIEW_DEPTH> m_viewStack;
    etl::atomic<bool> m_isTransitioning{false};
};
