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

#include "widgets/num_scroll/num_scroll.h"
#include "PixelUI.h"
#include <stdio.h>
#include "config.h"
#include <etl/string.h>

/**
 * @brief Constructor for NumScroll widget.
 * @param ui Reference to PixelUI instance for rendering and animation.
 */
NumScroll::NumScroll(PixelUI& ui, uint16_t x, uint16_t y, uint16_t w, uint16_t h) : 
    m_ui(ui),
    m_x(x),
    m_y(y),
    m_w(w),
    m_h(h)
 {
    setFocusable(true);
    updateGeometry();
 }

void NumScroll::updateGeometry() {
    if (presentation_ == Presentation::Bare) {
        setFocusInsets({2, static_cast<int32_t>(m_h) - 2, 2, 0});
    } else {
        setFocusInsets({1, 1, 1, 1});
    }
    setWidgetBounds({m_x, m_y, m_w, m_h});
}

/**
 * @brief Initialize widget with animation.
 */
void NumScroll::onLoad() {
    m_anim_offset = 0;
    m_is_active = false;

    anim_w = 0;
    anim_h = 0;

    updateGeometry();

    // Animate size from 0 to margin size
    m_ui.animate(anim_w, anim_h,
                 m_w, m_h,
                 200, EasingType::EASE_OUT_CUBIC,
                 PROTECTION::PROTECTED);
}

/**
 * @brief Initialize widget without animation (immediate size).
 */
void NumScroll::onLoadNoAnim() {
    m_anim_offset = 0;
    m_is_active = false;

    anim_w = 0;
    anim_h = 0;

    updateGeometry();

    // Set final size immediately
    anim_w = m_w;
    anim_h = m_h;
}

/**
 * @brief Clean up widget resources when offloaded.
 */
void NumScroll::onOffload() {
    m_ui.clearUnprotectedAnimations();
    m_is_active = false;
}

/**
 * @brief Handle selection of the widget.
 * @return True if widget requests input control, false otherwise.
 */
bool NumScroll::onSelect() {
    // Toggle active state
    if (!m_is_active) {
        m_ui.markDirty();
        return true; // Request input control
    } else {
        m_ui.markDirty();
        return false; // Return input control
    }
}

/**
 * @brief Handle user input events.
 * @param event Input event to process.
 * @return True if event consumed, false otherwise.
 */
bool NumScroll::handleEvent(InputEvent event) {
    if (!m_is_active) {
        return false;
    }

    // Update last interaction timestamp
    updateInteractionTime(m_ui.getCurrentTime());

    switch (event) {
        #ifdef NUMSCROLL_NAVI_DOWN
        case NUMSCROLL_NAVI_DOWN:
            decrementValue();
            return false;
        #endif

        #ifdef NUMSCROLL_NAVI_UP
        case NUMSCROLL_NAVI_UP:
            incrementValue();
            return false;
        #endif

        #ifdef NUMSCROLL_NAVI_SELECT
        case NUMSCROLL_NAVI_SELECT:
            m_ui.markDirty();
            return true;
        #endif

        default:
            return false;
    }
}

/**
 * @brief Render the scrollable number widget.
 */
Canvas& NumScroll::display() { return m_ui.getCanvas(); }

void NumScroll::drawSelf(const WidgetRenderContext& context) {
    Canvas& u8g2 = m_ui.getCanvas();

    // Compute animated drawing area (centered)
    int32_t draw_x = context.originX + m_x + (m_w - anim_w) / 2;
    int32_t draw_y = context.originY + m_y + (m_h - anim_h) / 2;
    int center_x = draw_x + anim_w / 2;
    int center_y = draw_y + anim_h / 2;

    // Clear background
    u8g2.setDrawColor(0);
    if (presentation_ == Presentation::Bare) {
        u8g2.drawBox(draw_x, draw_y, anim_w, anim_h);
    } else {
        u8g2.drawBox(draw_x + 2, draw_y + 2, anim_w - 4, anim_h - 4);
    }
    u8g2.setDrawColor(1);

    if (presentation_ == Presentation::Bare) {
        if (m_is_active) {
            u8g2.drawBox(draw_x + 2, draw_y + anim_h - 2, anim_w - 4, 2);
        }
    } else {
        // Draw border, thicker if active
        if (m_is_active) {
            u8g2.drawFrame(draw_x, draw_y, anim_w, anim_h);
            u8g2.drawFrame(draw_x + 1, draw_y + 1, anim_w - 2, anim_h - 2);
        } else {
            u8g2.drawFrame(draw_x, draw_y, anim_w, anim_h);
        }
    }

    // Clip drawing area inside the widget
    if (presentation_ == Presentation::Bare) {
        setClipWindow(context, {
            draw_x - context.originX,
            draw_y - context.originY,
            anim_w,
            anim_h - 2
        });
    } else {
        setClipWindow(context, {
            draw_x + 3 - context.originX,
            draw_y + 3 - context.originY,
            anim_w - 6,
            anim_h - 6
        });
    }

    u8g2.setFont(PIXELUI_FONT_NUMERIC);

    const int digit_height = 16;
    char buffer[16];

    // Draw current value with one previous and one next value for scroll effect
    for (int i = -1; i <= 1; i++) {
        int32_t value = m_current_value + i;
        if (value < m_min_value) value = m_min_value;
        if (value > m_max_value) value = m_max_value;

        // Format value for fixed digits if applicable
        formatValue(value, buffer, sizeof(buffer));

        int text_width = u8g2.getStrWidth(buffer);
        int y_pos = center_y + (i * digit_height) + m_anim_offset;

        // Center text horizontally
        u8g2.drawStr(center_x - text_width / 2, y_pos + 5, buffer);
    }

    restoreClipWindow(context);
    u8g2.setDrawColor(1);
}

/**
 * @brief Set the range of allowed numeric values.
 * @param min_val Minimum allowed value.
 * @param max_val Maximum allowed value.
 */
void NumScroll::setRange(int32_t min_val, int32_t max_val) {
    if (min_val <= max_val) {
        m_min_value = min_val;
        m_max_value = max_val;

        if (m_current_value < m_min_value) {
            m_current_value = m_min_value;
        }
        if (m_current_value > m_max_value) {
            m_current_value = m_max_value;
        }

        m_ui.markDirty();
    }
}

/**
 * @brief Set the current value and animate to it if changed.
 * @param val New value to set.
 */
void NumScroll::setValue(int32_t val) {
    if (val < m_min_value) val = m_min_value;
    if (val > m_max_value) val = m_max_value;

    if (val != m_current_value) {
        animateToValue(val);
    }
}

void NumScroll::setValueImmediate(int32_t val) {
    if (val < m_min_value) val = m_min_value;
    if (val > m_max_value) val = m_max_value;
    m_current_value = val;
    m_anim_offset = 0;
    m_ui.markDirty();
}

/**
 * @brief Increment the value by one step if not exceeding max.
 */
void NumScroll::incrementValue() {
    if (m_current_value < m_max_value) {
        animateToValue(m_current_value + 1);
    }
}

/**
 * @brief Decrement the value by one step if not below min.
 */
void NumScroll::decrementValue() {
    if (m_current_value > m_min_value) {
        animateToValue(m_current_value - 1);
    }
}

/**
 * @brief Animate the scroll effect to a new value.
 * @param new_value Target numeric value.
 */
void NumScroll::animateToValue(int32_t new_value) {
    int32_t diff = new_value - m_current_value;
    if (diff == 0) return;

    m_current_value = new_value;

    const int digit_height = 16;
    int32_t start_offset = (diff > 0) ? digit_height : -digit_height;

    // Start animation offset and animate to zero
    m_anim_offset = start_offset;
    m_ui.animate(m_anim_offset, 0, 200, EasingType::EASE_OUT_QUAD);

    m_ui.markDirty();
}

/**
 * @brief Format numeric value into buffer with optional leading zeros.
 * @param value Numeric value to format.
 * @param buffer Destination buffer.
 * @param buf_size Size of buffer.
 */
void NumScroll::formatValue(int32_t value, char* buffer, size_t buf_size) const {
    if (m_fixed_digits > 0) {
        char fmt[8];
        snprintf(fmt, sizeof(fmt), "%%0%dd", m_fixed_digits); // e.g. "%03d"
        snprintf(buffer, buf_size, fmt, (int)value);
    } else {
        snprintf(buffer, buf_size, "%d", (int)value);
    }
}
