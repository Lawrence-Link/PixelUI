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
#include <etl/atomic.h>
#include <etl/utility.h>

class ViewManager {
public:
    enum class LaunchResult {
        Ok,
        StackFull,
        ArenaFull,
        ConstructionFailed,
        TransitionInProgress,
    };

    ViewManager(PixelUI &ui) : m_ui(ui) {
        m_ui.setInputCallback ([this](InputEvent event) -> bool {
            if (isTransitioning()) {
                return false;
            }

            // Prioritize pop-up input - check for an active pop-up

            if (m_ui.popupCount() > 0U) {
                // The active Popup always gets the first chance to handle input.
                return m_ui.m_popupManager.handleTopPopupInput(event);
            }
            
            // If the pop-up did not handle the input or there is no pop-up, pass the input to the application at the top of the stack
            IApplication* current = m_applicationStack.top();
            if (current != nullptr) {
                return current->handleInput(event);
            }
            return false;
        });
    }
    ~ViewManager();

    ViewManager(const ViewManager&) = delete;
    ViewManager& operator=(const ViewManager&) = delete;

    template <typename T, typename... Args>
    LaunchResult push(Args&&... args) {
        TransitionGuard transition(*this);
        if (!transition.acquired()) {
            return LaunchResult::TransitionInProgress;
        }

        T* application = nullptr;
        const ApplicationStackResult result =
            m_applicationStack.emplace<T>(application, etl::forward<Args>(args)...);
        if (result != ApplicationStackResult::Ok) {
            return toLaunchResult(result);
        }

        activatePushedApplication(application);
        return LaunchResult::Ok;
    }

    LaunchResult launch(const AppItem& item, void* parameters = nullptr);
    bool pop();
    bool isTransitioning() const noexcept {
        return m_pendingEnter != nullptr ||
               m_isTransitioning.load(etl::memory_order_relaxed);
    }

    IApplication* getCurrentApp() const;
    size_t getViewDepth() const noexcept { return m_applicationStack.depth(); }
    size_t getArenaUsed() const noexcept { return m_applicationStack.used(); }
    static constexpr size_t getArenaCapacity() noexcept { return ApplicationStack::capacity(); }

private:
    class TransitionGuard {
    public:
        explicit TransitionGuard(ViewManager& manager) : manager_(manager) {
            if (manager_.m_pendingEnter != nullptr) {
                return;
            }
            bool expected = false;
            acquired_ = manager_.m_isTransitioning.compare_exchange_strong(
                expected,
                true,
                etl::memory_order_acquire,
                etl::memory_order_relaxed);
        }

        ~TransitionGuard() {
            if (acquired_) {
                manager_.m_isTransitioning.store(false, etl::memory_order_release);
            }
        }

        bool acquired() const noexcept { return acquired_; }

    private:
        ViewManager& manager_;
        bool acquired_ = false;
    };

    static LaunchResult toLaunchResult(ApplicationStackResult result);
    void activatePushedApplication(IApplication* application);
    void completePendingEnter();
    void clearNonOwningReferences();
    bool isTransitionCommitInProgress() const noexcept {
        return m_isTransitioning.load(etl::memory_order_relaxed);
    }

    friend class PixelUI;

    PixelUI &m_ui;
    ApplicationStack m_applicationStack;
    IApplication* m_pendingEnter = nullptr;
    etl::atomic<bool> m_isTransitioning{false};
};
