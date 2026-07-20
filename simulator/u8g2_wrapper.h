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
    U8G2Wrapper(){}
    U8G2Wrapper(int _width, int _height) : width(_width), height(_height) {};
    ~U8G2Wrapper() = default;

    void init();
    void drawTestString(const char* str);
    int getWidth()   { return this->U8G2::getDisplayWidth();}
    int getHeight()  { return this->U8G2::getDisplayHeight(); }
    Framebuffer getFramebufferPixels();

private:
    int width = 128;  // default width
    int height = 64;  // default height
};
