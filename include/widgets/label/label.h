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

enum class POS {
    TOP, BOTTOM, LEFT, RIGHT
};

class Label : public IWidget {
private:
    PixelUI& m_ui;
    int32_t m_x = 0, m_y = 0, m_w = 0, m_h = 0;
    const char* src;
    int32_t anim_w = 0, anim_h = 0;
    int32_t anim_x = 0, anim_y = 0;
    VoidCallback m_callback;
    POS load_pos;
    uint8_t const* m_font;
    void drawSelf(const WidgetRenderContext& context) override;
    Canvas& display() override;
public:
    Label(PixelUI& ui, uint16_t x, uint16_t y, const char* content, POS pos = POS::BOTTOM, const uint8_t* font = PIXELUI_FONT_TEXT);
    ~Label() = default;
    void onLoad() override;
    void onOffload() override;
    bool onSelect() override ;
    void setLoadPos(POS pos) {load_pos = pos;};
    void setCallback(VoidCallback cb) {m_callback = cb;}
    void setPosition(uint16_t x, uint16_t y) {
        m_x = x; m_y = y;
        setWidgetBounds({m_x, m_y, m_w, m_h});
    };

    void setSize(uint16_t w, uint16_t h) {
        m_w = w; m_h = h;
        setWidgetBounds({m_x, m_y, m_w, m_h});
    }
    
    void setText(const char* source) {src = source;};
};
