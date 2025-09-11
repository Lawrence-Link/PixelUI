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
#include <cstdint>
#include <functional>
#include <memory>

// 前向声明
class Animation;
enum class EasingType;
enum class PROTECTION;

class IPopupRenderer {
public:
    virtual ~IPopupRenderer() = default;
    virtual void markDirty() = 0;
    virtual uint32_t getCurrentTime() const = 0;
    virtual void addAnimation(std::shared_ptr<Animation> animation) = 0;
    virtual void markAnimationProtected(std::shared_ptr<Animation> animation) = 0;
    
    // 绘制接口
    virtual void setDrawColor(int color) = 0;
    virtual void drawRBox(int x, int y, int w, int h, int r) = 0;
    virtual void drawRFrame(int x, int y, int w, int h, int r) = 0;
    virtual void drawStr(int x, int y, const char* str) = 0;
    virtual void drawPixel(int x, int y) = 0;
    virtual void setFont(const uint8_t* font) = 0;
};