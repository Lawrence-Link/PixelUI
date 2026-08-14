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

#include "widgets/label/label.h"
#include "PixelUI.h"

/**
 * @brief Constructor for Label widget.
 * @param ui Reference to the PixelUI instance for rendering and animation.
 * @param x X coordinate of label reference point.
 * @param y Y coordinate of label reference point.
 * @param content Text content of the label.
 * @param pos Label alignment position relative to (x, y).
 */
Label::Label(PixelUI& ui, uint16_t x, uint16_t y, const char* content, POS pos, const uint8_t* font)
    : m_ui(ui), m_x(x), m_y(y), src(content), load_pos(pos), m_font(font)
{
    setFocusable(true);
    setWidgetBounds({m_x, m_y, m_w, m_h});
    setFocusBox(FocusBox(m_x + 1, m_y + 1, m_w - 2, m_h - 2));
}

/**
 * @brief Initialize the label when loaded. Calculates animated starting position
 *        based on alignment and triggers slide-in animation.
 */
void Label::onLoad() {
    U8G2& u8g2 = m_ui.getU8G2();
    u8g2.setFont(m_font);

    // Calculate font height
    int8_t font_height = u8g2.getFontAscent() + u8g2.getFontDescent();

    // Set initial animated position based on alignment
    switch (load_pos) {
        case POS::TOP: {
            anim_x = m_x;
            anim_y = m_y - font_height; 
            m_ui.animate(anim_y, m_y, 300, EasingType::EASE_OUT_CUBIC, PROTECTION::NOT_PROTECTED);
        } break;
        case POS::BOTTOM: {
            anim_x = m_x;
            anim_y = m_y + font_height; 
            m_ui.animate(anim_y, m_y, 300, EasingType::EASE_OUT_CUBIC, PROTECTION::NOT_PROTECTED);
        } break;
        case POS::LEFT: {
            anim_x = m_x - u8g2.getUTF8Width(src);
            anim_y = m_y; 
            m_ui.animate(anim_x, m_x, 300, EasingType::EASE_OUT_CUBIC, PROTECTION::NOT_PROTECTED);
        } break;
        case POS::RIGHT: {
            anim_x = m_x + u8g2.getUTF8Width(src);
            anim_y = m_y; 
            m_ui.animate(anim_x, m_x, 300, EasingType::EASE_OUT_CUBIC, PROTECTION::NOT_PROTECTED);
        } break;
    }
}

/**
 * @brief Clean up resources when label is offloaded.
 */
void Label::onOffload() {
    // No dynamic resources to release
}

/**
 * @brief Render the label text on screen with clipping to avoid overflow.
 */
U8G2& Label::display() { return m_ui.getU8G2(); }

void Label::drawSelf(const WidgetRenderContext& context) {
    if (!src) return;

    U8G2& u8g2 = m_ui.getU8G2();
    u8g2.setFont(PIXELUI_FONT_TEXT);

    // Compute font metrics
    int8_t font_ascent = u8g2.getAscent();
    int8_t font_descent = u8g2.getDescent();
    int8_t font_height = font_ascent - font_descent;

    // Compute text width
    int32_t text_width = u8g2.getUTF8Width((const char*)src);

    // Clip drawing area to label rectangle
    setClipWindow(context, {m_x, m_y - font_height, text_width, font_height + 1});

    // Draw text at animated position
    u8g2.drawUTF8(context.originX + anim_x, context.originY + anim_y, (const char*)src);

    // Reset clipping
    restoreClipWindow(context);
}

/**
 * @brief Handle label selection. Triggers callback if defined.
 * @return False, indicating event propagation continues.
 */
bool Label::onSelect() {
    if (m_callback) {
        m_callback();
    }
    return false;
}
