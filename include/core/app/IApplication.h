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

#include "PixelUI.h"
#include "functional"

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