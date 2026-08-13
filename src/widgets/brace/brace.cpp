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

#include "widgets/brace/brace.h"
#include "PixelUI.h"

/**
 * @brief Construct a new Brace object
 * * @param ui Reference to the PixelUI instance
 * * Note: pos_x_ and pos_y_ now represent the top-left corner.
 */
Brace::Brace(PixelUI& ui, uint16_t pos_x, uint16_t pos_y, uint16_t size_w, uint16_t size_h) : 
    m_ui(ui), 
    pos_x_(pos_x),
    pos_y_(pos_y),
    size_w_(size_w),
    size_h_(size_h) 
{
    setWidgetBounds({pos_x_, pos_y_, size_w_, size_h_});
    int32_t start_anim_x = (size_w_ / 2);
    int32_t start_anim_y = (size_h_ / 2);
    
    anim_w = 0; 
    anim_h = 0;

    anim_x = start_anim_x;
    anim_y = start_anim_y;

    setFocusBox(FocusBox(pos_x_ + 1, pos_y_ + 1, size_w_ - 1, size_h_ - 1));
}

/**
 * @brief Initialize animations for brace expansion and set the focus box
 */
void Brace::onLoad() {
    int32_t start_anim_x = (size_w_ / 2);
    int32_t start_anim_y = (size_h_ / 2);
    
    anim_w = 0; 
    anim_h = 0;
    
    anim_x = start_anim_x;
    anim_y = start_anim_y;

    m_ui.animate(anim_w, size_w_, 550, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    m_ui.animate(anim_h, size_h_, 600, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    
    m_ui.animate(anim_x, 0, 550, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    m_ui.animate(anim_y, 0, 600, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
}

/**
 * @brief Called when the widget is removed/offloaded
 */
void Brace::onOffload() {
    // No special cleanup required
}

/**
 * @brief Draw the brace with corners and optional content inside
 */
U8G2& Brace::display() { return m_ui.getU8G2(); }

void Brace::drawSelf(const WidgetRenderContext& context) {
    U8G2& u8g2 = m_ui.getU8G2();

    int tl_x = context.originX + pos_x_ + anim_x;
    int tl_y = context.originY + pos_y_ + anim_y;
    int current_w = anim_w;
    int current_h = anim_h;

    /**< Set the clipping window to restrict drawing inside the brace, based on top-left */
    setClipWindow(context, {
        tl_x - context.originX,
        tl_y - context.originY,
        current_w,
        current_h
    });

    /**< Draw the user-provided content inside the brace, if any */
    if (contentWithinBrace) {
        contentWithinBrace();
    }

    /**< Reset clipping to full screen */
    restoreClipWindow(context);

    /**< Draw corner lines for the brace */
    
    // Top-left corner: (tl_x, tl_y)
    u8g2.drawLine(tl_x, tl_y, tl_x + 4, tl_y);
    u8g2.drawLine(tl_x, tl_y, tl_x, tl_y + 4);

    // Top-right corner: (tl_x + current_w, tl_y)
    u8g2.drawLine(tl_x + current_w, tl_y, tl_x + current_w - 4, tl_y);
    u8g2.drawLine(tl_x + current_w, tl_y, tl_x + current_w, tl_y + 4);

    // Bottom-left corner: (tl_x, tl_y + current_h)
    u8g2.drawLine(tl_x, tl_y + current_h, tl_x + 4, tl_y + current_h);
    u8g2.drawLine(tl_x, tl_y + current_h, tl_x, tl_y + current_h - 4);

    // Bottom-right corner: (tl_x + current_w, tl_y + current_h)
    u8g2.drawLine(tl_x + current_w, tl_y + current_h, tl_x + current_w - 4, tl_y + current_h);
    u8g2.drawLine(tl_x + current_w, tl_y + current_h, tl_x + current_w, tl_y + current_h - 4);
}
