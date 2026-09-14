#include "PixelUI.h"
#include "widgets/label/label.h"

#include <string.h>

namespace {

constexpr size_t FRAME_SIZE = 1024U;

void drawFrame(U8G2& display, Label& label, uint8_t* frame) {
    display.clearBuffer();
    label.draw();
    memcpy(frame, display.getBufferPtr(), FRAME_SIZE);
}

bool hasPixelOutsideViewport(const uint8_t* frame, int32_t rightEdge) {
    for (int32_t y = 0; y < 64; ++y) {
        for (int32_t x = rightEdge; x < 128; ++x) {
            const size_t index = static_cast<size_t>(x) +
                                 static_cast<size_t>(y / 8) * 128U;
            if ((frame[index] & static_cast<uint8_t>(1U << (y & 7))) != 0U) {
                return true;
            }
        }
    }
    return false;
}

int verifyClippingAndAutomaticScroll() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    ui.begin();
    Label label(ui, 4, 12, 8, "ABCDE", POS::BOTTOM, u8g2_font_4x6_tr);
    label.setOverflow(Label::Overflow::AutoScroll);
    label.onLoad();

    ui.tickFromISR(300U);
    ui.process();

    uint8_t initialFrame[FRAME_SIZE] = {};
    uint8_t scrolledFrame[FRAME_SIZE] = {};
    uint8_t endFrame[FRAME_SIZE] = {};
    uint8_t returningFrame[FRAME_SIZE] = {};
    uint8_t resetFrame[FRAME_SIZE] = {};
    drawFrame(display, label, initialFrame);
    if (hasPixelOutsideViewport(initialFrame, 12)) return 1;
    if (!ui.needsHeartbeat()) return 2;

    ui.tickFromISR(800U);
    ui.process();
    drawFrame(display, label, scrolledFrame);
    if (memcmp(initialFrame, scrolledFrame, FRAME_SIZE) != 0) return 3;

    ui.tickFromISR(50U);
    ui.process();
    drawFrame(display, label, scrolledFrame);
    if (memcmp(initialFrame, scrolledFrame, FRAME_SIZE) == 0) return 4;
    if (hasPixelOutsideViewport(scrolledFrame, 12)) return 5;

    ui.tickFromISR(10000U);
    ui.process();
    drawFrame(display, label, endFrame);
    ui.tickFromISR(500U);
    ui.process();
    ui.tickFromISR(50U);
    ui.process();
    drawFrame(display, label, returningFrame);
    if (memcmp(endFrame, returningFrame, FRAME_SIZE) == 0) return 6;
    if (memcmp(initialFrame, returningFrame, FRAME_SIZE) == 0) return 7;
    if (hasPixelOutsideViewport(returningFrame, 12)) return 8;

    ui.tickFromISR(10000U);
    ui.process();
    drawFrame(display, label, resetFrame);
    if (memcmp(initialFrame, resetFrame, FRAME_SIZE) != 0) return 9;

    label.onOffload();
    if (ui.needsHeartbeat()) return 10;
    return 0;
}

int verifyShortTextRemainsIdle() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    ui.begin();
    Label label(ui, 4, 12, 30, "A", POS::BOTTOM, u8g2_font_4x6_tr);
    label.setOverflow(Label::Overflow::AutoScroll);
    label.onLoad();

    ui.tickFromISR(300U);
    ui.process();
    if (ui.needsHeartbeat()) return 1;

    label.setText("ABCDE");
    label.setViewportWidth(8U);
    if (!ui.needsHeartbeat()) return 2;

    label.clearViewportWidth();
    if (label.getViewportWidth() != 0U || ui.needsHeartbeat()) return 3;
    return 0;
}

int verifyDisabledFallback() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    ui.begin();
    Label label(ui, 4, 12, 8, "ABCDE", POS::BOTTOM, u8g2_font_4x6_tr);
    label.setOverflow(Label::Overflow::AutoScroll);
    label.onLoad();

    ui.tickFromISR(300U);
    ui.process();
    uint8_t frame[FRAME_SIZE] = {};
    drawFrame(display, label, frame);
    if (hasPixelOutsideViewport(frame, 12)) return 1;
    return ui.needsHeartbeat() ? 2 : 0;
}

} // namespace

int main() {
#if !PIXELUI_USE_LABEL_SCROLL
    return verifyDisabledFallback();
#else
    const int scrolling = verifyClippingAndAutomaticScroll();
    if (scrolling != 0) return scrolling;

    const int idle = verifyShortTextRemainsIdle();
    if (idle != 0) return 20 + idle;
    return 0;
#endif
}
