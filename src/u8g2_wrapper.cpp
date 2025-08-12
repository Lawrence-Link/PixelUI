#include "u8g2_wrapper.h"
#include "u8x8.h"

void U8G2Wrapper::init() {
    // Initialize u8g2 structure with your desired display IC preset.
    u8g2_Setup_ssd1306_128x64_noname_f( 
        &u8g2,
        U8G2_R0,
        u8x8_byte_empty,
        u8x8_dummy_cb
    );

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    // after initialization

    width = getWidth();
    height = getHeight();
}

void U8G2Wrapper::drawTestString(const char* str) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(&u8g2, 0, 10, str);
    u8g2_SendBuffer(&u8g2);
}

std::vector<std::vector<bool>> U8G2Wrapper::getFramebufferPixels() {
    uint8_t* buffer = u8g2_GetBufferPtr(&u8g2);
    size_t buffer_size = u8g2_GetBufferSize(&u8g2);

    int bytes_per_column = (height + 7) / 8;
    
    // pixel buffer is width * height, each column has `bytes_per_column` bytes.
    std::vector<std::vector<bool>> pixels(height, std::vector<bool>(width, false));

    for (int x = 0; x < width; ++x) {
        for (int byte_row = 0; byte_row < bytes_per_column; ++byte_row) {
            // calculate the index in the buffer, each column has `bytes_per_column` bytes. every byte represents 8 pixels vertically.
            size_t byte_index = x + byte_row * width;
            if (byte_index >= buffer_size) {
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
