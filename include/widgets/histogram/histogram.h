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
#include "../ChartBuffer.h"
#include <etl/limits.h>

class Histogram : public IWidget {
public:
    template <size_t N, uint16_t ExpandedWidth, uint16_t ExpandedHeight>
    Histogram(
        PixelUI& ui,
        uint16_t pos_x,
        uint16_t pos_y,
        uint16_t size_w,
        uint16_t size_h,
        float (&buffer)[N],
        ChartExpandSize<ExpandedWidth, ExpandedHeight>,
        EXPAND_BASE base,
        const char* label = nullptr)
        : Histogram(
              ui,
              pos_x,
              pos_y,
              size_w,
              size_h,
              buffer,
              N,
              ExpandedWidth,
              ExpandedHeight,
              base,
              label) {
        static_assert(N == ExpandedWidth,
                      "Histogram buffer size must equal the expanded chart width");
        static_assert(N <= static_cast<size_t>(etl::numeric_limits<int>::max()),
                      "Histogram buffer capacity exceeds supported index range");
    }

    ~Histogram() = default;

    void onLoad() override;
    void onOffload() override;
    bool onSelect() override;
    bool handleEvent(InputEvent event) override;

    void setSize(uint16_t mar_w, uint16_t mar_h) { 
        size_w_ = mar_w; size_h_ = mar_h; 
        setWidgetBounds({pos_x_, pos_y_, size_w_, size_h_});
        setFocusBox(FocusBox(pos_x_ + 1, pos_y_ + 1, size_w_ - 1, size_h_ - 1));
    }
    void setPosition(uint16_t pos_x, uint16_t pos_y) { 
        pos_x_ = pos_x; pos_y_ = pos_y;
        setWidgetBounds({pos_x_, pos_y_, size_w_, size_h_});
        setFocusBox(FocusBox(pos_x_ + 1, pos_y_ + 1, size_w_ - 1, size_h_ - 1));
    }

    bool isExpanded() const { return is_expanded; }

    void addData(float value);

    // statistics within the window
    float getMaxValueInWindow() const;
    float getAverageValueInWindow() const;
    float getMinValueInWindow() const;

    // statistics throughout the history
    float getMaxValueInHistory() const;
    float getAverageValueInHistory() const;
    float getMinValueInHistory() const;

    void clearData();

private:
    PixelUI& m_ui;

    uint16_t pos_x_ = 0, pos_y_ = 0;
    uint16_t size_w_ = 0, size_h_ = 0;
    uint16_t exp_w = 0, exp_h = 0;
    EXPAND_BASE base_;

    // Internal data buffer for real-time data streaming
    float* m_data_buffer = nullptr;
    int m_buffer_size = 0;
    int m_write_index = 0;
    int m_data_count = 0;
    
    // Statistics tracking (window)
    float m_max_value = 0.0f;
    float m_min_value = 0.0f;
    float m_sum_value = 0.0f;

    // Statistics tracking (history - all time)
    float m_hist_max_value = 0.0f;
    float m_hist_min_value = etl::numeric_limits<float>::max();
    float m_hist_sum_value = 0.0f;
    uint32_t m_hist_count = 0;

    // Cache for visible window statistics
    float m_cached_visible_max = 0.0f;
    float m_cached_visible_min = 0.0f;
    int m_cached_visible_width = 0;
    bool m_visible_cache_dirty = true;

    int32_t anim_w = 0;
    int32_t anim_h = 0;
    int32_t anim_x = 0;
    int32_t anim_y = 0;
    
    bool is_expanded = false;  // Add this line
    
    const char* m_label = nullptr;

    Histogram(
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
        const char* label);

    void expandWidget();
    void contractWidget();
    void calculateExpandPosition(int32_t& target_x, int32_t& target_y);
    void initializeDataBuffer();
    void updateStatistics(float new_value, float old_value, bool replacing_data);
    void recalculateExtremes();
    void drawHistogramData(int tl_x, int tl_y, int width, int height, U8G2& u8g2);
    void drawSelf(const WidgetRenderContext& context) override;
    U8G2& display() override;
};
