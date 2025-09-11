/*
 * Copyright (C) 2025 Lawrence Li
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

#pragma once

#include "../widgets.h"

class Brace : public Widget {
public:
    Brace(PixelUI& ui);
    ~Brace() = default;

    void onLoad() override;
    void onOffload() override;
    void draw() override;

    void setMargin(uint16_t mar_w, uint16_t mar_h) {margin_w_ = mar_w; margin_h_ = mar_h;}
    void setCoordinate(uint16_t coord_x, uint16_t coord_y) {coord_x_ = coord_x; coord_y_=coord_y;}
    void setDrawContentFunction(std::function<void()> func) { contentWithinBrace = func; }
private:
    uint16_t coord_x_ = 0, coord_y_ = 0;
    uint16_t margin_w_ = 0, margin_h_ = 0;
    PixelUI& m_ui;

    std::function<void()> contentWithinBrace;
    // animation related varibles:
    float anim_w = 0;
    float anim_h = 0;
    float anim_x = 0;
    float anim_y = 0;
};