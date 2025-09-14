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

#pragma once

#include "u8g2.h"
#include <vector>
#include "../third_party/u8g2/cppsrc/U8g2lib.h"

class U8G2Wrapper : public U8G2 {
public:
    U8G2Wrapper(){}
    U8G2Wrapper(int _width, int _height) : width(_width), height(_height) {};
    ~U8G2Wrapper() = default;

    void init();
    void drawTestString(const char* str);
    int getWidth()   { return this->U8G2::getDisplayWidth();}
    int getHeight()  { return this->U8G2::getDisplayHeight(); }
    std::vector<std::vector<bool>> getFramebufferPixels();

private:
    int width = 128;  // default width
    int height = 64;  // default height
};
