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

#include "widgets/histogram/histogram.h"
#include "PixelUI.h"
#include "calc/TextAlignHelper/TextAlignHelper.h"
#include <math.h>
#include <etl/algorithm.h>
#include <etl/limits.h>

/**
 * @brief Constructor for Histogram widget.
 * @param ui Reference to the PixelUI instance for rendering and animation.
 * @param pos_x X coordinate of the widget's top-left corner.
 * @param pos_y Y coordinate of the widget's top-left corner.
 * @param size_w Width of the widget.
 * @param size_h Height of the widget.
 * @param size_w_exp Expanded width of the widget when expanded.
 * @param size_h_exp Expanded height of the widget when expanded.
 * @param base Expansion anchor base.
 */
Histogram::Histogram(
    PixelUI& ui,
    uint16_t pos_x,
    uint16_t pos_y,
    uint16_t size_w,
    uint16_t size_h,
    float* buffer,
    size_t buffer_size,
    uint16_t size_w_exp,
    uint16_t size_h_exp,
    EXPAND_BASE base,
    const char* label
) : 
    m_ui(ui), 
    pos_x_(pos_x), 
    pos_y_(pos_y),
    size_w_(size_w),
    size_h_(size_h),
    exp_w(size_w_exp),
    exp_h(size_h_exp),
    base_(base),
    m_data_buffer(buffer),
    m_buffer_size(static_cast<int>(buffer_size)),
    m_label(label)
{
    setWidgetBounds({pos_x_, pos_y_, size_w_, size_h_});
    // pos_x_ and pos_y_ now represent the top-left anchor point.
    int32_t start_anim_x = (size_w_ / 2);
    int32_t start_anim_y = (size_h_ / 2);

    anim_w = 4; 
    anim_h = 4;

    anim_x = start_anim_x;
    anim_y = start_anim_y;

    setFocusBox(FocusBox(pos_x_ + 1, pos_y_ + 1, size_w_ - 1, size_h_ - 1));
}

/**
 * @brief Initialize widget when loaded. Sets up animations, focus, and internal buffer.
 */
void Histogram::onLoad() {
    int32_t start_anim_x = (size_w_ / 2);
    int32_t start_anim_y = (size_h_ / 2);

    anim_w = 0; 
    anim_h = 0;

    anim_x = start_anim_x;
    anim_y = start_anim_y;

    m_ui.animate(anim_w, size_w_, 550, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_h, size_h_, 600, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);

    m_ui.animate(anim_x, 0, 550, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_y, 0, 600, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    

    // Initialize the internal data buffer
    initializeDataBuffer();
}

/**
 * @brief Initialize the circular data buffer and statistics.
 */
void Histogram::initializeDataBuffer() {
    // Clear buffer
    for (int i = 0; i < m_buffer_size; ++i) {
        m_data_buffer[i] = 0.0f;
    }

    // Reset statistics
    m_write_index = 0;
    m_data_count = 0;
    m_max_value = 0.0f;
    m_sum_value = 0.0f;
    m_min_value = etl::numeric_limits<float>::max();
    
    // Reset history statistics
    m_hist_max_value = 0.0f;
    m_hist_min_value = etl::numeric_limits<float>::max();
    m_hist_sum_value = 0.0f;
    m_hist_count = 0;
    
    // Reset visible cache
    m_cached_visible_max = 0.0f;
    m_cached_visible_min = 0.0f;
    m_cached_visible_width = 0;
    m_visible_cache_dirty = true;
}

/**
 * @brief Free resources when widget is offloaded.
 */
void Histogram::onOffload() {
    
}

/**
 * @brief Add a new data point to the histogram buffer.
 * @param value The new float value to add.
 */
void Histogram::addData(float value) {
    // Store the old value at current position for statistics update
    float old_value = m_data_buffer[m_write_index];
    bool replacing_valid_data = (m_data_count >= m_buffer_size);
    
    // Add new value to buffer
    m_data_buffer[m_write_index] = value;
    
    // Update write index (ring buffer)
    m_write_index = (m_write_index + 1) % m_buffer_size;
    
    // Update data count
    if (m_data_count < m_buffer_size) {
        m_data_count++;
    }
    
    // Mark visible cache as dirty
    m_visible_cache_dirty = true;
    
    // Update statistics efficiently
    updateStatistics(value, old_value, replacing_valid_data);
}

/**
 * @brief Update max, min, and sum statistics efficiently.
 * @param new_value The new value being added.
 * @param old_value The old value being replaced (if any).
 * @param replacing_data True if replacing an existing value in a full buffer.
 */
void Histogram::updateStatistics(float new_value, float old_value, bool replacing_data) {
    // Update history statistics (all-time)
    m_hist_max_value = etl::max(m_hist_max_value, new_value);
    if (m_hist_min_value == etl::numeric_limits<float>::max() || new_value < m_hist_min_value) {
        m_hist_min_value = new_value;
    }
    m_hist_sum_value += new_value;
    m_hist_count++;

    // Update window statistics
    if (!replacing_data) {
        // Buffer not full, simple update
        m_sum_value += new_value;
        m_max_value = etl::max(m_max_value, new_value);
        // Only update min if a valid (non-max) value is added or if min is still at its maximum possible value
        if (m_min_value == etl::numeric_limits<float>::max() || new_value < m_min_value) {
            m_min_value = new_value;
        }
    } else {
        // Replace existing value
        m_sum_value = m_sum_value - old_value + new_value;

        // Recalculate extremes if old value was max or min (recalculating min if old_value was the placeholder max is safer)
        if (old_value == m_max_value || old_value == m_min_value || m_min_value == etl::numeric_limits<float>::max()) {
            recalculateExtremes();
        } else {
            m_max_value = etl::max(m_max_value, new_value);
            m_min_value = etl::min(m_min_value, new_value);
        }
    }
}

/**
 * @brief Recalculate the max and min values by scanning the buffer.
 */
void Histogram::recalculateExtremes() {
    if (m_data_count == 0) {
        m_max_value = 0.0f;
        m_min_value = etl::numeric_limits<float>::max(); // Reset min to max
        m_sum_value = 0.0f;
        return;
    }

    m_max_value = m_data_buffer[0];
    m_min_value = m_data_buffer[0];

    for (int i = 1; i < m_data_count; ++i) {
        m_max_value = etl::max(m_max_value, m_data_buffer[i]);
        m_min_value = etl::min(m_min_value, m_data_buffer[i]);
    }
}

/**
 * @brief Get the maximum value in the histogram.
 * @return Maximum float value in buffer.
 */
float Histogram::getMaxValueInWindow() const {
    return m_max_value;
}

/**
 * @brief Get the average value of the histogram.
 * @return Average float value or 0 if buffer empty.
 */
float Histogram::getAverageValueInWindow() const {
    if (m_data_count == 0) {
        return 0.0f;
    }
    return m_sum_value / m_data_count;
}

/**
 * @brief Get the minimum value in the histogram.
 * @return Minimum float value in buffer (or 0.0f if empty).
 */
float Histogram::getMinValueInWindow() const {
    if (m_data_count == 0) return 0.0f;
    return m_min_value;
}

/**
 * @brief Get the maximum value in the entire history.
 * @return Maximum float value ever recorded.
 */
float Histogram::getMaxValueInHistory() const {
    return m_hist_max_value;
}

/**
 * @brief Get the average value of all historical data.
 * @return Average float value across all recorded data or 0 if no data.
 */
float Histogram::getAverageValueInHistory() const {
    if (m_hist_count == 0) {
        return 0.0f;
    }
    return m_hist_sum_value / m_hist_count;
}

/**
 * @brief Get the minimum value in the entire history.
 * @return Minimum float value ever recorded (or 0.0f if no data).
 */
float Histogram::getMinValueInHistory() const {
    if (m_hist_count == 0) return 0.0f;
    if (m_hist_min_value == etl::numeric_limits<float>::max()) return 0.0f;
    return m_hist_min_value;
}

/**
 * @brief Clear all data in the histogram buffer and reset statistics.
 */
void Histogram::clearData() {
    for (int i = 0; i < m_buffer_size; ++i) {
        m_data_buffer[i] = 0.0f;
    }
    m_write_index = 0;
    m_data_count = 0;
    m_max_value = 0.0f;
    m_sum_value = 0.0f;
    m_min_value = etl::numeric_limits<float>::max();
    
    // Reset history statistics
    m_hist_max_value = 0.0f;
    m_hist_min_value = etl::numeric_limits<float>::max();
    m_hist_sum_value = 0.0f;
    m_hist_count = 0;
    
    // Reset visible cache
    m_cached_visible_max = 0.0f;
    m_cached_visible_min = 0.0f;
    m_cached_visible_width = 0;
    m_visible_cache_dirty = true;
}

/**
 * @brief Handle user input events.
 * @param event Input event from UI system.
 * @return True if the event was handled.
 */
bool Histogram::handleEvent(InputEvent event) {
    if (event == InputEvent::SELECT) {
        is_expanded = false;
        contractWidget();
        return true;
    }
    return false;
}

/**
 * @brief Handle widget selection toggle.
 * @return True after processing selection.
 */
bool Histogram::onSelect() {
    m_ui.clearUnprotectedAnimations();
    if (!is_expanded) {
        expandWidget();
        is_expanded = true;
    } else {
        contractWidget();
        is_expanded = false;
    }
    return true;
}

/**
 * @brief Animate widget expansion to target size and position.
 */
void Histogram::expandWidget() {
    int32_t target_x, target_y;
    calculateExpandPosition(target_x, target_y);

    m_ui.animate(anim_w, exp_w, 400, EasingType::EASE_OUT_QUAD);
    m_ui.animate(anim_h, exp_h, 350, EasingType::EASE_OUT_QUAD);
    // Animate the Top-Left position to the calculated target offsets
    m_ui.animate(anim_x, target_x, 400, EasingType::EASE_OUT_QUAD);
    m_ui.animate(anim_y, target_y, 350, EasingType::EASE_OUT_QUAD);
}

/**
 * @brief Animate widget contraction back to original size and position.
 */
void Histogram::contractWidget() {
    m_ui.animate(anim_w, size_w_, 350, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_h, size_h_, 400, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    // Animate the Top-Left position back to (0, 0) offset
    m_ui.animate(anim_x, 0, 350, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_y, 0, 400, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
}

/**
 * @brief Calculate target animation offsets for expansion based on the base corner.
 * The offsets (anim_x, anim_y) are applied to the top-left corner (pos_x_, pos_y_).
 * @param target_x Output target x offset.
 * @param target_y Output target y offset.
 */
void Histogram::calculateExpandPosition(int32_t& target_x, int32_t& target_y) {
    int32_t width_diff = exp_w - size_w_;
    int32_t height_diff = exp_h - size_h_;

    switch (base_) {
        case EXPAND_BASE::TOP_LEFT:
            // Top-Left corner (pos_x_ + anim_x) should stay fixed at pos_x_
            target_x = 0;
            target_y = 0;
            break;
        case EXPAND_BASE::TOP_RIGHT:
            // Top-Right corner (pos_x_ + anim_x + anim_w) should stay fixed
            target_x = -width_diff;
            target_y = 0;
            break;
        case EXPAND_BASE::BOTTOM_LEFT:
            // Bottom-Left corner (pos_y_ + anim_y + anim_h) should stay fixed
            target_x = 0;
            target_y = -height_diff;
            break;
        case EXPAND_BASE::BOTTOM_RIGHT:
            // Bottom-Right corner should stay fixed
            target_x = -width_diff;
            target_y = -height_diff;
            break;
    }
}

/**
 * @brief Render the histogram widget on the screen.
 */
Canvas& Histogram::display() { return m_ui.getCanvas(); }

void Histogram::drawSelf(const WidgetRenderContext& context) {
    Canvas& u8g2 = m_ui.getCanvas();
    
    // tl_x and tl_y are the animated top-left corner coordinates
    int tl_x = context.originX + pos_x_ + anim_x;
    int tl_y = context.originY + pos_y_ + anim_y;
    int current_w = anim_w;
    int current_h = anim_h;

    // Clear background area (relative to top-left)
    u8g2.setDrawColor(0);
    u8g2.drawBox(tl_x + 2, tl_y, current_w - 4, current_h);
    u8g2.setDrawColor(1);

    // --- Draw border corners ---
    // Top-Left: (tl_x, tl_y)
    u8g2.drawLine(tl_x, tl_y, tl_x + 4, tl_y);
    u8g2.drawLine(tl_x, tl_y, tl_x, tl_y + 4);
    
    // Top-Right: (tl_x + current_w, tl_y)
    u8g2.drawLine(tl_x + current_w, tl_y, tl_x + current_w - 4, tl_y);
    u8g2.drawLine(tl_x + current_w, tl_y, tl_x + current_w, tl_y + 4); 

    // Bottom-Left: (tl_x, tl_y + current_h)
    u8g2.drawLine(tl_x, tl_y + current_h, tl_x + 4, tl_y + current_h);
    u8g2.drawLine(tl_x, tl_y + current_h, tl_x, tl_y + current_h - 4);
    
    // Bottom-Right: (tl_x + current_w, tl_y + current_h)
    u8g2.drawLine(tl_x + current_w, tl_y + current_h, tl_x + current_w - 4, tl_y + current_h);
    u8g2.drawLine(tl_x + current_w, tl_y + current_h, tl_x + current_w, tl_y + current_h - 4);

    // Draw vertical borders
    u8g2.drawLine(tl_x, tl_y, tl_x, tl_y + current_h);
    u8g2.drawLine(tl_x + 1, tl_y, tl_x + 1, tl_y + current_h);
    u8g2.drawLine(tl_x + current_w, tl_y, tl_x + current_w, tl_y + current_h);
    u8g2.drawLine(tl_x + current_w - 1, tl_y, tl_x + current_w - 1, tl_y + current_h);

    // Draw histogram bars, passing top-left coordinates, current width, and height
    drawHistogramData(tl_x, tl_y, current_w, current_h, u8g2);

    // Draw label if provided (positioned in the top-right area)
    if (m_label != nullptr) {
        u8g2.setFont(PIXELUI_FONT_CHART);
        
        // Define label area in top-right corner (matching original offset logic)
        Rect label_area = {
            static_cast<int16_t>(tl_x + current_w - 20 - 2),  // Right area
            static_cast<int16_t>(tl_y + 2),               // Top margin
            20,                                            // Width for label area
            6                                              // Height matches font height
        };
        
        // Calculate text position using TextAlignHelper
        // Use Top alignment for Y to position near top, Right alignment for X
        TextPos text_pos = TextAlignHelper::calcTextPos(
            u8g2.rawDisplay().getU8g2(),
            label_area,
            m_label,
            TextAlignX::Right,
            TextAlignY::Top
        );
        
        u8g2.drawStr(text_pos.x, text_pos.y, m_label);
    }
}

/**
 * @brief Draw the bars representing histogram data.
 * @param tl_x Widget top-left x coordinate.
 * @param tl_y Widget top-left y coordinate.
 * @param width Current width of the widget.
 * @param height Current height of the widget.
 * @param u8g2 Reference to U8G2 for drawing.
 */
void Histogram::drawHistogramData(int tl_x, int tl_y, int width, int height, Canvas& u8g2) {
    if (m_data_count == 0) {
        return;
    }

    // Determine number of points to draw based on current width
    // Subtract 3 to account for 2px borders (width - 3 = drawable columns from right border to left border inclusive)
    int points_to_draw = etl::min(width - 3, static_cast<int>(m_data_count));
    if (points_to_draw <= 0) return;

    // Calculate visible window min/max (with caching)
    if (m_visible_cache_dirty || m_cached_visible_width != points_to_draw) {
        m_cached_visible_max = -etl::numeric_limits<float>::max();
        m_cached_visible_min = etl::numeric_limits<float>::max();
        
        for (int i = 0; i < points_to_draw; ++i) {
            int buffer_offset = i + 1;
            int data_index = (m_write_index - buffer_offset + m_buffer_size) % m_buffer_size;
            if (buffer_offset <= m_data_count) {
                float value = m_data_buffer[data_index];
                m_cached_visible_max = etl::max(m_cached_visible_max, value);
                m_cached_visible_min = etl::min(m_cached_visible_min, value);
            }
        }
        
        m_cached_visible_width = points_to_draw;
        m_visible_cache_dirty = false;
    }

    // Use cached visible window max for scaling
    float visible_max = m_cached_visible_max;
    
    // Use visible max for scaling, or fallback to buffer max
    float scale_factor = (visible_max > 0.0f) 
        ? static_cast<float>(height - 2) / visible_max
        : static_cast<float>(height - 2) / m_max_value;

    // Draw bars from right (newest) to left (oldest)
    // Start from right edge minus 2px border
    for (int i = 0; i < points_to_draw; ++i) {
        int buffer_offset = i + 1;
        int data_index = (m_write_index - buffer_offset + m_buffer_size) % m_buffer_size;

        if (buffer_offset <= m_data_count) {
            float value = m_data_buffer[data_index];
            int bar_height = static_cast<int>(value * scale_factor);

            // Clamp bar height to available vertical space
            bar_height = etl::min(bar_height, height - 4);
            bar_height = etl::max(bar_height, 0);

            // Calculate bar position: start from right border (tl_x + width - 2), move left
            int bar_x = tl_x + width - 2 - i;
            int bar_y = tl_y + height - bar_height;

            // Draw vertical line for the bar
            if (bar_height > 0) {
                u8g2.drawVLine(bar_x, bar_y, bar_height);
            }
        }
    }
}
