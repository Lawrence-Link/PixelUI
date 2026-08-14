#include "PixelUI.h"
#include "widgets/label/label.h"

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);

    Label label(ui, 0, 12, "Label", POS::BOTTOM, u8g2_font_4x6_tr);
    label.onLoad();

    display.setFont(PIXELUI_FONT_TEXT);
    label.draw();

    return display.getU8g2()->font == u8g2_font_4x6_tr ? 0 : 1;
}
