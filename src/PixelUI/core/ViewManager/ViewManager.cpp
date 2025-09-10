#include "PixelUI/core/ViewManager/ViewManager.h"

void ViewManager::push(std::shared_ptr<IApplication> app) {
    if (!app) return;
        std::lock_guard<std::mutex> lock(m_stackMutex);
        m_isTransitioning = true;

        if (!m_viewStack.empty()) {
            m_viewStack.top()->onPause(); // 将当前栈顶应用暂停
        }

        m_viewStack.push(app);          // 压入app
        m_ui.setDrawable(app);    // 绘制/Input控制权移交给当前app

        app->onEnter([this]() {this->pop();}); // 给app传入退出pop回调方法
        m_ui.markDirty();

        m_isTransitioning = false;
}

void ViewManager::pop() {
    std::lock_guard<std::mutex> lock(m_stackMutex);

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
    m_isTransitioning = false;
}

std::shared_ptr<IApplication> ViewManager::getCurrentApp() const {
    std::lock_guard<std::mutex> lock(m_stackMutex);
    if (m_viewStack.empty()) return nullptr;
    return m_viewStack.top();
}