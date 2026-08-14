#include "PixelUI.h"
#include "core/animation/animation.h"
#include "ui/Popup/PopupValueDigits.h"

namespace {

bool pixelIsSet(U8G2& display, int32_t x, int32_t y) {
    const uint8_t* buffer = u8g2_GetBufferPtr(display.getU8g2());
    const uint16_t width = display.getDisplayWidth();
    return (buffer[x + (y / 8) * width] & (1U << (y % 8))) != 0U;
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);

    int callbackCount = 0;
    int32_t callbackValue = -1;
    int32_t value = 123;
    PopupValueDigits popup(
        ui, 80, 40, value, 4, "Value", 0,
        [&callbackCount, &callbackValue](int32_t updated) {
            ++callbackCount;
            callbackValue = updated;
        });

    popup.update(ui.getCurrentTime());
    ui.Heartbeat(100);
    popup.update(ui.getCurrentTime());
    display.clearBuffer();
    popup.draw();

    constexpr int32_t fixedPointOne = 1 << 10;
    const int32_t progress = 100 * fixedPointOne / 300;
    const int32_t eased = EasingCalculator::calculate(EasingType::EASE_OUT_CUBIC, progress);
    const int32_t currentWidth = static_cast<int32_t>(
        (static_cast<int64_t>(80 << 10) * eased) / fixedPointOne) >> 10;
    const int32_t currentHeight = currentWidth * 40 / 80;
    const int32_t x0 = 64 - currentWidth / 2;
    const int32_t y0 = 32 - currentHeight / 2;
    const int32_t x1 = x0 + currentWidth;
    const int32_t y1 = y0 + currentHeight;
    for (int32_t y = 0; y < 64; ++y) {
        for (int32_t x = 0; x < 128; ++x) {
            if (pixelIsSet(display, x, y) &&
                (x < x0 || x >= x1 || y < y0 || y >= y1)) return 1;
        }
    }

    ui.Heartbeat(200);
    popup.update(ui.getCurrentTime());
    popup.handleInput(InputEvent::RIGHT);
    popup.handleInput(InputEvent::SELECT);
    popup.handleInput(InputEvent::RIGHT);
    if (value != 1123 || callbackCount != 1 || callbackValue != 1123) return 2;

    popup.handleInput(InputEvent::BACK);
    popup.handleInput(InputEvent::RIGHT);
    popup.handleInput(InputEvent::SELECT);
    popup.handleInput(InputEvent::LEFT);
    if (value != 1023 || callbackCount != 2 || callbackValue != 1023) return 3;

    int32_t clamped = 1234567;
    PopupValueDigits sixDigits(ui, 100, 40, clamped, 6, "", 0);
    if (clamped != 999999) return 4;
    if (PopupValueDigits::isValidDigitCount(0) ||
        PopupValueDigits::isValidDigitCount(MAX_INT_FIXED_WIDTH + 1U)) return 5;

    return 0;
}
