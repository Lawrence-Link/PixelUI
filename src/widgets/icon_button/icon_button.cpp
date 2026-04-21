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

#include "widgets/icon_button/icon_button.h"

/**
 * @brief Constructor for IconButton widget.
 * @param ui Reference to the PixelUI instance for rendering and animation.
 */
IconButton::IconButton(PixelUI& ui, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char* source) : 
    m_ui(ui),
    pos_x(x),
    pos_y(y),
    m_w(w),
    m_h(h),
    src(source)
{
    setFocusable(true);
    IWidget::setFocusBox(FocusBox(pos_x - 1, pos_y - 1, m_w + 2, m_h + 2));
}

/**
 * @brief Initialize the widget when loaded. Sets initial animation offsets.
 */
void IconButton::onLoad() {
    // Set initial animated position slightly above the target y
    // anim_x = pos_x;
    // anim_y = pos_y - 10;

    // Animate Y position to slide down into final position
}

/**
 * @brief Clean up resources when widget is offloaded.
 */
void IconButton::onOffload() {
    // No dynamic resources allocated, nothing to clean
}

/**
 * @brief Render the icon button on the screen.
 */
void IconButton::draw() {
    if (!src) {
        return; // No image source, nothing to draw
    }

    U8G2& u8g2 = m_ui.getU8G2();

    // Draw XBM bitmap at current animated position
    u8g2.drawXBM(pos_x, pos_y, m_w, m_h, src);
}

/**
 * @brief Handle button selection (click).
 * @return False, indicating event propagation is not stopped.
 */
bool IconButton::onSelect() {
    // Trigger user-defined callback if available
    if (m_callback) {
        m_callback();
    }
    return false;
}
