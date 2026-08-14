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

#include "u8g2_wrapper.h"
#include "u8x8.h"

U8G2Wrapper::U8G2Wrapper() {
    u8g2_Setup_ssd1306_128x64_noname_f(
        &u8g2, U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    width = U8G2::getDisplayWidth();
    height = U8G2::getDisplayHeight();
    bufferSize = u8g2_GetBufferSize(&u8g2);
}

void U8G2Wrapper::init() {
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

}

void U8G2Wrapper::drawTestString(const char* str) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(&u8g2, 0, 10, str);
    u8g2_SendBuffer(&u8g2);
}

Framebuffer U8G2Wrapper::getFramebufferPixels() {
    uint8_t* buffer = u8g2_GetBufferPtr(&u8g2);
    int bytes_per_column = (height + 7) / 8;
    
    // pixel buffer is width * height, each column has `bytes_per_column` bytes.
    Framebuffer pixels{};

    for (int x = 0; x < width; ++x) {
        for (int byte_row = 0; byte_row < bytes_per_column; ++byte_row) {
            // calculate the index in the buffer, each column has `bytes_per_column` bytes. every byte represents 8 pixels vertically.
            size_t byte_index = x + byte_row * width;
            if (byte_index >= bufferSize) {
                continue;
            }
            uint8_t byte = buffer[byte_index]; 
            for (int bit = 0; bit < 8; ++bit) {
                int y = byte_row * 8 + bit;
                if (y >= height) break;
                pixels[y][x] = (byte >> bit) & 1; // fill the buffer with the pixel state.
            }
        }
    }

    return pixels;
}
