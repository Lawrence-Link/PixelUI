#include "PixelUI.h"
#include "core/NumericFormatter.h"
#include "core/NumericRange.h"
#include "widgets/num_scroll/num_scroll.h"

#include <string.h>

namespace {

ValueCallback noOpCallback() {
    return [](int32_t) {};
}

void drawWidget(NumScroll& widget, U8G2& display) {
    display.clearBuffer();
    widget.draw({0, 0, {0, 0, 128, 64}});
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);

    NumericRange range;
    if (!NumericRange::tryCreate(INT32_MIN, INT32_MAX, 1, range)) return 1;
    NumScroll sizeWidget(
        ui, 0, 0, 24, 16, range, NumericFormatter::integer());
    sizeWidget.onLoad();
    if (ui.activeAnimationCount() != 1U) return 2;
    sizeWidget.onOffload();
    if (ui.activeAnimationCount() != 0U) return 3;

    NumScroll widget(ui, 10, 10, 48, 20, range, NumericFormatter::integer());
    widget.onLoadNoAnim();
    widget.setValueImmediate(INT32_MIN);
    drawWidget(widget, display);
    widget.setValueImmediate(INT32_MAX);
    drawWidget(widget, display);

    for (size_t i = 0; i < MAX_ANIMATION_COUNT; ++i) {
        if (!ui.animateCallback(
                0, 1, 1000, EasingType::LINEAR, noOpCallback())) return 4;
    }

    widget.setValueImmediate(INT32_MIN);
    widget.setValue(INT32_MAX);
    drawWidget(widget, display);
    uint8_t failedAnimationFrame[1024]{};
    memcpy(failedAnimationFrame, u8g2_GetBufferPtr(display.getU8g2()),
           sizeof(failedAnimationFrame));

    widget.setValueImmediate(INT32_MAX);
    drawWidget(widget, display);
    if (widget.getValue() != INT32_MAX ||
        memcmp(failedAnimationFrame, u8g2_GetBufferPtr(display.getU8g2()),
               sizeof(failedAnimationFrame)) != 0) return 5;

    const size_t unrelatedAnimations = ui.activeAnimationCount();
    widget.onOffload();
    if (ui.activeAnimationCount() != unrelatedAnimations) return 6;

    return 0;
}
