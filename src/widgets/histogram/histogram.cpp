/*
 * Copyright (C) 2025 Lawrence Link
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "widgets/histogram/histogram.h"
#include "ui/Popup/Popup.h"

Histogram::Histogram(PixelUI& ui) : m_ui(ui) {
    onLoad();
};

void Histogram::onLoad() {
    // 初始化动画坐标为0（相对于原始位置）
    anim_x = 0;
    anim_y = 0;
    
    // 初始化尺寸动画
    m_ui.animate(anim_w, margin_w_, 550, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_h, margin_h_, 600, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
}

void Histogram::onOffload() {
    // m_ui.animate(anim_w, 0, 300, EasingType::EASE_IN_CUBIC, PROTECTION::PROTECTED);
    // m_ui.animate(anim_h, 0, 300, EasingType::EASE_IN_CUBIC, PROTECTION::PROTECTED);
}

bool Histogram::handleEvent(InputEvent event) {
    if (event == InputEvent::SELECT) {
        is_expanded = false;
        contractWidget();
        return true;
    }
    return false;
}

bool Histogram::onSelect(){
    m_ui.clearUnprotectedAnimations();
    if (!is_expanded) {
        // 展开动画
        expandWidget();
        is_expanded = true;
    } else {
        // 收缩动画
        contractWidget();
        is_expanded = false;
    }
    return true;
}

void Histogram::expandWidget() {
    int32_t target_x, target_y;
    calculateExpandPosition(target_x, target_y);
    
    // 动画到展开尺寸和位置
    m_ui.animate(anim_w, exp_w, 400, EasingType::EASE_OUT_QUAD);
    m_ui.animate(anim_h, exp_h, 350, EasingType::EASE_OUT_QUAD);
    m_ui.animate(anim_x, target_x, 400, EasingType::EASE_OUT_QUAD);
    m_ui.animate(anim_y, target_y, 350, EasingType::EASE_OUT_QUAD);
}

void Histogram::contractWidget() {
    // 动画回原始尺寸和位置
    m_ui.animate(anim_w, margin_w_, 350, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_h, margin_h_, 400, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_x, 0, 350, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_y, 0, 400, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
}

void Histogram::calculateExpandPosition(int32_t& target_x, int32_t& target_y) {
    // 计算展开后的中心位置偏移
    int32_t width_diff = exp_w - margin_w_;
    int32_t height_diff = exp_h - margin_h_;
    
    switch (base_) {
        case EXPAND_BASE::TOP_LEFT:
            target_x = width_diff / 2;
            target_y = height_diff / 2;
            break;
            
        case EXPAND_BASE::TOP_RIGHT:
            target_x = -width_diff / 2;
            target_y = height_diff / 2;
            break;
            
        case EXPAND_BASE::BOTTOM_LEFT:
            target_x = width_diff / 2;
            target_y = -height_diff / 2;
            break;
            
        case EXPAND_BASE::BOTTOM_RIGHT:
            target_x = -width_diff / 2;
            target_y = -height_diff / 2;
            break;
    }
}

void Histogram::setData(float* data_ptr, uint16_t data_size, uint16_t head_index) {
    m_data_ptr = data_ptr;
    m_data_size = data_size;
    m_head_index = head_index;
}

void Histogram::draw() {
    U8G2& u8g2 = m_ui.getU8G2();

    // 使用动画坐标和尺寸
    int current_x = coord_x_ + anim_x;
    int current_y = coord_y_ + anim_y;
    int half_width = anim_w / 2;
    int half_height = anim_h / 2;
    
    u8g2.setDrawColor(1);
    if (is_expanded) {
        u8g2.clearDisplay();
        u8g2.drawStr(3, 10, "<STATS>");
        u8g2.drawStr(3, 20, "Max:");
        u8g2.drawStr(3, 30, "1.45uSv/h");
        u8g2.drawStr(3, 40, "Min:");
        u8g2.drawStr(3, 50, "0.25uSv/h");
    } 
    u8g2.setDrawColor(0);
    u8g2.drawBox(current_x - half_width + 2, current_y - half_height, 2 * half_width - 4, 2 * half_height);
    u8g2.setDrawColor(1);

    // 绘制边框角点
    // Top left corner
    u8g2.drawLine(current_x - half_width, current_y - half_height, 
                  current_x - half_width + 4, current_y - half_height);
    u8g2.drawLine(current_x - half_width, current_y - half_height, 
                  current_x - half_width, current_y - half_height + 4);

    // Top right corner
    u8g2.drawLine(current_x + half_width, current_y - half_height, 
                  current_x + half_width - 4, current_y - half_height);
    u8g2.drawLine(current_x + half_width, current_y - half_height, 
                  current_x + half_width, current_y - half_height + 4);

    // Bottom left corner
    u8g2.drawLine(current_x - half_width, current_y + half_height, 
                  current_x - half_width + 4, current_y + half_height);
    u8g2.drawLine(current_x - half_width, current_y + half_height, 
                  current_x - half_width, current_y + half_height - 4);

    // Bottom right corner
    u8g2.drawLine(current_x + half_width, current_y + half_height, 
                  current_x + half_width - 4, current_y + half_height);
    u8g2.drawLine(current_x + half_width, current_y + half_height, 
                  current_x + half_width, current_y + half_height - 4);
    
    // 绘制边框 - 使用动画坐标
    // Left vertical line with thickness of 2
    u8g2.drawLine(current_x - half_width, current_y - half_height, 
                  current_x - half_width, current_y + half_height);
    u8g2.drawLine(current_x - half_width + 1, current_y - half_height, 
                  current_x - half_width + 1, current_y + half_height);

    // Right vertical line with thickness of 2
    u8g2.drawLine(current_x + half_width, current_y - half_height, 
                  current_x + half_width, current_y + half_height);
    u8g2.drawLine(current_x + half_width - 1, current_y - half_height, 
                  current_x + half_width - 1, current_y + half_height);

    // 绘制直方图数据
    if (m_data_ptr != nullptr && m_data_size > 0) {
        uint16_t points_to_draw = anim_w < m_data_size ? static_cast<uint16_t>(anim_w) : m_data_size;
        int start_index = (m_head_index + m_data_size - points_to_draw) % m_data_size;
        float max_value = 1.0f; 

        for (int i = 0; i < points_to_draw; ++i) {
            int data_index = (start_index + i) % m_data_size;
            float value = m_data_ptr[data_index];
            int bar_height = static_cast<int>((value / max_value) * anim_h);

            int x_pos = current_x - static_cast<int>(anim_w / 2.0f) + i;
            int y_start = current_y + static_cast<int>(anim_h / 2.0f);
            int y_end = y_start - bar_height;

            u8g2.drawLine(x_pos, y_start, x_pos, y_end);
        }
    }
    
    // 绘制标签
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(current_x + half_width - 19, current_y - half_height + 7, "Hist");
}