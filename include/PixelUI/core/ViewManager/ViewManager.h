#pragma once

#include "PixelUI/core/app/IApplication.h"
#include <stack>
#include <memory>

class ViewManager {
public:
    ViewManager(PixelUI &ui) : m_ui(ui) {
        m_ui.setInputCallback ([this](InputEvent event) -> bool {
            if (!m_viewStack.empty()) {
                return m_viewStack.top()->handleInput(event); 
            }
            return false;
        });
    }

    void push(std::shared_ptr<IApplication> app);
    void pop();
    
private:
    PixelUI &m_ui;
    std::stack<std::shared_ptr<IApplication>> m_viewStack;
};