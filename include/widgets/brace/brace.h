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

class Brace : public IWidget {

public:
    Brace(PixelUI& ui, uint16_t pos_x, uint16_t pos_y, uint16_t size_w, uint16_t size_h);
    ~Brace() = default;

    void onLoad() override;
    void onOffload() override;
    void draw() override;
    bool onSelect() override { if (m_callback) m_callback(); return false;}

    void setCallback(std::function<void()> cb) { m_callback = cb; };
    void setSize(uint16_t mar_w, uint16_t mar_h) {
        size_h_ = mar_w; size_h_ = mar_h;
        setFocusBox(FocusBox(pos_x_ + 1, pos_y_ + 1, size_w_ - 1, size_h_ - 1));
    }
    void setPosition(int16_t pos_x, int16_t pos_y) {
        pos_x_ = pos_x; pos_y_=pos_y;
        setFocusBox(FocusBox(pos_x_ + 1, pos_y_ + 1, size_w_ - 1, size_h_ - 1));
    }
    void setDrawContentFunction(std::function<void()> func) { contentWithinBrace = func; }
    
private:
    PixelUI& m_ui;
    int16_t pos_x_ = 0, pos_y_ = 0;
    int16_t size_w_ = 0, size_h_ = 0;

    std::function<void()> contentWithinBrace;
    std::function<void()> m_callback = nullptr;

    int32_t anim_w = 0;
    int32_t anim_h = 0;
    int32_t anim_x = 0;
    int32_t anim_y = 0;
};