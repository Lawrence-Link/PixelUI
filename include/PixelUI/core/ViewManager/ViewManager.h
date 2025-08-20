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

    void push(std::shared_ptr<IApplication> app) {
        if (!app) return;
        
        if (!m_viewStack.empty()) {
            m_viewStack.top()->onPause(); // 将当前栈顶应用暂停
        }

        m_viewStack.push(app);          // 压入app
        m_ui.setDrawable(app);    // 绘制/Input控制权移交给当前app

        app->onEnter([this]() {this->pop();}); // 给app传入退出pop回调方法
        m_ui.markDirty();
    }
    void pop() {
        if (m_viewStack.empty()) return;
        
        m_viewStack.top()->onExit(); // 调用应用onExit方法
        m_viewStack.pop(); 

        if (!m_viewStack.empty()) {
            auto& previousApp = m_viewStack.top();
            m_ui.setDrawable( previousApp );
            previousApp->onResume(); // 恢复
        }
        else {
            m_ui.setDrawable(nullptr);
        }
        m_ui.markDirty();
    }

private:
    PixelUI &m_ui;
    std::stack<std::shared_ptr<IApplication>> m_viewStack;
};