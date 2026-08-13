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

#include "../IWidget.h"

class TextButton : public IWidget {
private:
    VoidCallback m_callback;          // Callback when selected

    PixelUI& m_ui;

    int32_t m_x = 0, m_y = 0;                  // Position of the button
    int32_t m_w = 0, m_h = 0;                  // Size of the button                       // Reference to the UI manager

    const char* src = nullptr;                 // Button text
    // Animation values
    int32_t anim_w = 0, anim_h = 0;           // Animated width/height
    int32_t anim_x = 0, anim_y = 0;           // Animated position

    void drawSelf(const WidgetRenderContext& context) override;
    U8G2& display() override;

public:
    TextButton(PixelUI& ui, uint16_t x, uint16_t y, uint16_t w, uint16_t h, char const* text = "")
        : m_ui(ui), m_x(x), m_y(y), m_w(w), m_h(h), src(text)
    {
        setFocusable(true);
        setWidgetBounds({m_x, m_y, m_w, m_h});
        setFocusBox({m_x, m_y, m_w, m_h});
    }
    ~TextButton() override = default;

    // IWidget overrides
    void onLoad() override;
    void onOffload() override;
    bool onSelect() override;

    // Setters
    void setCallback(VoidCallback cb) { m_callback = cb; }
    void setPosition(uint16_t x, uint16_t y) {
        m_x = x; m_y = y; 
        setWidgetBounds({m_x, m_y, m_w, m_h});
        setFocusBox(FocusBox(m_x + 1, m_y + 1, m_w - 2, m_h - 2)); // reconfigure the focusbox
    }
    void setSize(uint16_t w, uint16_t h) {
        m_w = w; m_h = h; 
        setWidgetBounds({m_x, m_y, m_w, m_h});
        setFocusBox(FocusBox(m_x + 1, m_y + 1, m_w - 2, m_h - 2)); // reconfigure the focusbux
    }
    void setText(const char* text) { src = text; }
};
