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

/**
 * @class NumScroll
 * @brief A scrollable numeric input widget with vertical scrolling animation
 * 
 * This widget displays a number in a square box and allows users to increment
 * or decrement the value using LEFT/RIGHT input events. The scrolling effect
 * is achieved through vertical animation of digit sprites.
 */
class NumScroll : public IWidget {
public:
    enum class Presentation : uint8_t {
        Framed,
        Bare
    };

    NumScroll(PixelUI& ui, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    ~NumScroll() = default;

    void onLoad() override;
    void onLoadNoAnim();
    void onOffload() override;
    bool handleEvent(InputEvent event) override;
    bool onSelect() override;
    uint32_t getTimeout() const override { return TIMEOUT_MS; }

    void setPosition(uint16_t x, uint16_t y) {
        m_x = x; m_y = y; 
        updateGeometry();
    }

    void setRange(int32_t min_val, int32_t max_val);
    void setValue(int32_t val);
    void setValueImmediate(int32_t val);
    int32_t getValue() const { return m_current_value; }

    void setFixedIntDigits(uint8_t digits) { m_fixed_digits = digits; }
    void setPresentation(Presentation presentation) {
        presentation_ = presentation;
        updateGeometry();
    }
    void setSize(uint16_t w, uint16_t h) {
        m_w = w; m_h = h; 
        updateGeometry();
    }

private:
    PixelUI& m_ui;

    uint16_t m_x = 0;
    uint16_t m_y = 0;

    uint16_t m_w = 24;
    uint16_t m_h = 16;

    int32_t m_current_value = 0;
    int32_t m_min_value = 0;
    int32_t m_max_value = 99;
    uint8_t m_fixed_digits = 0; 
    Presentation presentation_ = Presentation::Framed;

    int32_t m_anim_offset = 0;
    
    int32_t anim_w = 0;
    int32_t anim_h = 0;

    static constexpr uint32_t TIMEOUT_MS = 5000;

    void drawSelf(const WidgetRenderContext& context) override;
    U8G2& display() override;

    void incrementValue();
    void decrementValue();
    void animateToValue(int32_t new_value);
    void updateGeometry();

    void formatValue(int32_t value, char* buffer, size_t buf_size) const;
};
