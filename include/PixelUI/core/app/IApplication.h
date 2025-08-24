#pragma once

#include "PixelUI/pixelui.h"
#include "functional"

class IDrawable;
class IInputHandler;

class IApplication : public IDrawable, public IInputHandler {
public:

    using ExitCallback = std::function<void()>; // 退出回调函数
    virtual ~IApplication() = default;

    // 大循环
    virtual void loop() {};
    // 应用被推入视图栈顶
    virtual void onEnter(ExitCallback exitCallback) { m_exitCallback = exitCallback; }
    // 本体被弹出时
    virtual void onExit() {}; 
    // 新应用被压入栈顶时   
    virtual void onPause() {};   
    // 栈顶应用被弹出时
    virtual void onResume() {};  

protected:
    void requestExit() {
        if (m_exitCallback) {
            m_exitCallback();
        }
    }

private:
    ExitCallback m_exitCallback;
};