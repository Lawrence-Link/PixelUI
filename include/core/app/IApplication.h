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

#include "PixelUI.h"
#include <etl/inplace_function.h>
#include "config.h"

class IApplication : public IDrawable, public IInputHandler {
    friend class ViewManager;

public:

    using ExitCallback = etl::inplace_function<void(), CALLBACK_STORAGE_SIZE>; // Exit callback function
    virtual ~IApplication() = default;

    // Called when the app is pushed to the top of the stack
    virtual void onEnter(ExitCallback exitCallback) { m_exitCallback = exitCallback; }
    // Called when the app is exited
    virtual void onExit() {}; 
    // Called when the app is paused by another app being pushed on top
    virtual void onPause() {};   
    // Called when the top app is popped and this app resumes
    virtual void onResume() {}; 

protected:
    void requestExit() {
        if (m_exitCallback) {
            m_exitCallback();
        }
    }

private:
    void clearExitCallback() { m_exitCallback = nullptr; }

    ExitCallback m_exitCallback;
};
