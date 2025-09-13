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

#include "PixelUI/core/widgets/histogram/histogram.h"

Histogram::Histogram(PixelUI& ui) : m_ui(ui) {
    onLoad();
};

void Histogram::onLoad() {
    m_ui.animate(anim_w, margin_w_, 550, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
    m_ui.animate(anim_h, margin_h_, 600, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
}

void Histogram::onOffload() {
    // m_ui.animate(anim_w, 0, 300, EasingType::EASE_IN_CUBIC, PROTECTION::PROTECTED);
    // m_ui.animate(anim_h, 0, 300, EasingType::EASE_IN_CUBIC, PROTECTION::PROTECTED);
}

void Histogram::setData(float* data_ptr, uint16_t data_size, uint16_t head_index) {
    m_data_ptr = data_ptr;
    m_data_size = data_size;
    m_head_index = head_index;
}

void Histogram::draw() {
    U8G2& u8g2 = m_ui.getU8G2();

    int half_width = anim_w / 2;
    int half_height = anim_h / 2;

    // Top left corner
    u8g2.drawLine(coord_x_ - half_width, coord_y_ - half_height, coord_x_ - half_width + 4, coord_y_ - half_height);
    u8g2.drawLine(coord_x_ - half_width, coord_y_ - half_height, coord_x_ - half_width, coord_y_ - half_height + 4);

    // Top right corner
    u8g2.drawLine(coord_x_ + half_width, coord_y_ - half_height, coord_x_ + half_width - 4, coord_y_ - half_height);
    u8g2.drawLine(coord_x_ + half_width, coord_y_ - half_height, coord_x_ + half_width, coord_y_ - half_height + 4);

    // Bottom left corner
    u8g2.drawLine(coord_x_ - half_width, coord_y_ + half_height, coord_x_ - half_width + 4, coord_y_ + half_height);
    u8g2.drawLine(coord_x_ - half_width, coord_y_ + half_height, coord_x_ - half_width, coord_y_ + half_height - 4);

    // Bottom right corner
    u8g2.drawLine(coord_x_ + half_width, coord_y_ + half_height, coord_x_ + half_width - 4, coord_y_ + half_height);
    u8g2.drawLine(coord_x_ + half_width, coord_y_ + half_height, coord_x_ + half_width, coord_y_ + half_height - 4);
    
    // 绘制厚度为2的左侧竖线
    u8g2.drawLine(coord_x_ - half_width, coord_y_ - half_height, coord_x_ - half_width, coord_y_ + half_height);
    u8g2.drawLine(coord_x_ - half_width + 1, coord_y_ - half_height, coord_x_ - half_width + 1, coord_y_ + half_height);

    // 绘制厚度为2的右侧竖线
    u8g2.drawLine(coord_x_ + half_width, coord_y_ - half_height, coord_x_ + half_width, coord_y_ + half_height);
    u8g2.drawLine(coord_x_ + half_width - 1, coord_y_ - half_height, coord_x_ + half_width - 1, coord_y_ + half_height);

    uint16_t points_to_draw = anim_w < m_data_size ? static_cast<uint16_t>(anim_w) : m_data_size;

    // 计算数据起始索引，用于显示最新数据
    int start_index = (m_head_index + m_data_size - points_to_draw) % m_data_size;

    // 假设数据范围是 0 到 1.0f
    float max_value = 1.0f; 

    for (int i = 0; i < points_to_draw; ++i) {
        // 从循环缓冲区中获取数据
        int data_index = (start_index + i) % m_data_size;
        float value = m_data_ptr[data_index];

        // 将数据值映射到高度
        int bar_height = static_cast<int>((value / max_value) * anim_h);

        // 绘制直方图柱
        // x_pos 从左边缘开始，逐个像素向右
        int x_pos = coord_x_ - static_cast<int>(anim_w / 2.0f) + i;
        // y_pos 从底部向上绘制
        int y_start = coord_y_ + static_cast<int>(anim_h / 2.0f);
        int y_end = y_start - bar_height;

        u8g2.drawLine(x_pos, y_start, x_pos, y_end);
    }
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(coord_x_ + half_width - 19, coord_y_ - half_height + 7, "Hist");
}