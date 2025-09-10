#pragma once

#include "PixelUI/core/app/IApplication.h"
#include <stack>
#include <memory>
#include <mutex>
#include <atomic>

class ViewManager {
public:
    ViewManager(PixelUI &ui) : m_ui(ui) {
        m_ui.setInputCallback ([&](InputEvent event) -> bool {
            if (!m_viewStack.empty()) {
                return m_viewStack.top()->handleInput(event); // app on the top of stack handles input
            }
            return false;
        });
    }

    void push(std::shared_ptr<IApplication> app);
    void pop();

    bool isTransitioning() const noexcept {
        return m_isTransitioning.load(std::memory_order_relaxed);
    }

    std::shared_ptr<IApplication> getCurrentApp() const;
private:
    PixelUI &m_ui;
    std::stack<std::shared_ptr<IApplication>> m_viewStack;
    mutable std::mutex m_stackMutex;
    std::atomic<bool> m_isTransitioning{false};
};