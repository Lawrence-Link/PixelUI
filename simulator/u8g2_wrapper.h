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

#include "u8g2.h"
#include <etl/array.h>
#include "../third_party/u8g2/cppsrc/U8g2lib.h"

constexpr size_t FRAMEBUFFER_WIDTH = 128;
constexpr size_t FRAMEBUFFER_HEIGHT = 64;
using FramebufferRow = etl::array<bool, FRAMEBUFFER_WIDTH>;
using Framebuffer = etl::array<FramebufferRow, FRAMEBUFFER_HEIGHT>;

class U8G2Wrapper : public U8G2 {
public:
    /** @brief Creates a wrapper configured for a 128x64 SSD1306 framebuffer. */
    U8G2Wrapper();

    /**
     * @brief Creates a wrapper with explicit logical dimensions.
     * @param _width Logical framebuffer width in pixels.
     * @param _height Logical framebuffer height in pixels.
     */
    U8G2Wrapper(int _width, int _height) : width(_width), height(_height) {};

    /** @brief Destroys the wrapper. */
    ~U8G2Wrapper() = default;

    /** @brief Initializes the configured display and leaves power-save mode. */
    void init();

    /**
     * @brief Clears the buffer and draws a test string at the top-left corner.
     * @param str Null-terminated string to draw.
     */
    void drawTestString(const char* str);

    /** @return The logical framebuffer width in pixels. */
    int getWidth() const { return width; }

    /** @return The logical framebuffer height in pixels. */
    int getHeight() const { return height; }

    /** @return A pixel-oriented copy of the current U8G2 framebuffer. */
    Framebuffer getFramebufferPixels();

private:
    int width = 128;  // default width
    int height = 64;  // default height
    size_t bufferSize = 0;
};
