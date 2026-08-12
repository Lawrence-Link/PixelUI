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

#include <stdint.h>
#include "U8g2lib.h"
#include "core/Callbacks.h"
#include "core/CommonTypes.h"

class PixelUI;

class IWidget{
private:
    bool focusable = false;
    bool Selected = false;
    FocusBox focus;
protected:
    // activation related
    bool m_is_active = false;
    uint32_t m_last_interaction_time = 0;
    void setFocusBox(const FocusBox& pos) {focus = pos;}
public:
    virtual ~IWidget() = default;
    virtual void draw() = 0;

    /**
     * @brief Handles an input event.
     * @param event The event code.
     * @return true if the widget has completed its input handling and wants to return control to the FocusManager.
     */
    virtual bool handleEvent(InputEvent event) { return false; }

    virtual void onLoad() = 0;
    virtual void onOffload() = 0;

    /**
     * @brief Triggers the onSelect action.
     * @return true if the widget wants to take over input control, false otherwise.
     */
    virtual bool onSelect() { return false; }

    /**
     * @brief Gets the timeout duration in milliseconds.
     * @return Timeout duration. Return 0 to disable timeout.
     */
    virtual uint32_t getTimeout() const { return 0; }

    /**
     * @brief Called when the widget becomes active (takes over input control).
     */
    virtual void onActivate(uint32_t currentTime) {
        m_is_active = true;
        m_last_interaction_time = currentTime;
    }

    /**
     * @brief Called when the widget is deactivated (returns control to FocusManager).
     */
    virtual void onDeactivate() {
        m_is_active = false;
    }

    /**
     * @brief Updates the last interaction time.
     */
    void updateInteractionTime(uint32_t currentTime) {
        m_last_interaction_time = currentTime;
    }

    /**
     * @brief Gets the last interaction time.
     */
    uint32_t getLastInteractionTime() const {
        return m_last_interaction_time;
    }

    /**
     * @brief Checks if the widget is currently active.
     */
    bool isActive() const { return m_is_active; }

    /**
     * @brief Checks if the widget is focusable
     */
    bool isFocusable() { return focusable; }
    void setFocusable(bool state) { focusable = state; }

    FocusBox getFocusBox() { return focus; }
};
