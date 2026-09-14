#include "PixelUI.h"
#include "widgets/ChartSeries.h"
#include "widgets/analog_clock/analog_clock.h"
#include "widgets/brace/brace.h"
#include "widgets/curve_chart/curve_chart.h"
#include "widgets/histogram/histogram.h"
#include "widgets/label/label.h"
#include "widgets/num_scroll/num_scroll.h"
#include "widgets/text_button/text_button.h"

#include <string.h>

namespace {

constexpr size_t FRAME_SIZE = 1024U;

#if PIXELUI_USE_ANIMATION
ValueCallback noOpCallback() {
    return [](int32_t) {};
}
#endif

bool frameHasPixels(const uint8_t* frame) {
    for (size_t i = 0; i < FRAME_SIZE; ++i) {
        if (frame[i] != 0U) return true;
    }
    return false;
}

template <typename Widget>
bool verifyLoadTransition(Widget& widget, PixelUI& ui, U8G2& display) {
    uint8_t immediateFrame[FRAME_SIZE]{};
    uint8_t animatedFrame[FRAME_SIZE]{};
#if PIXELUI_USE_ANIMATION
    uint8_t fallbackFrame[FRAME_SIZE]{};
#endif

    ui.clearAllAnimations();
    widget.onLoad(LoadTransition::Immediate);
    if (ui.activeAnimationCount() != 0U) return false;
    display.clearBuffer();
    widget.draw();
    memcpy(immediateFrame, display.getBufferPtr(), FRAME_SIZE);
    if (!frameHasPixels(immediateFrame)) return false;

    widget.onLoad();
#if PIXELUI_USE_ANIMATION
    if (ui.activeAnimationCount() == 0U) return false;
    ui.tickFromISR(1200U);
    ui.process();
#else
    if (ui.activeAnimationCount() != 0U) return false;
#endif
    display.clearBuffer();
    widget.draw();
    memcpy(animatedFrame, display.getBufferPtr(), FRAME_SIZE);
    if (memcmp(immediateFrame, animatedFrame, FRAME_SIZE) != 0) return false;

#if PIXELUI_USE_ANIMATION
    while (ui.activeAnimationCount() < MAX_ANIMATION_COUNT) {
        if (!ui.animateCallback(
                0, 1, 1000U, EasingType::LINEAR, noOpCallback())) {
            return false;
        }
    }
    widget.onLoad();
    display.clearBuffer();
    widget.draw();
    memcpy(fallbackFrame, display.getBufferPtr(), FRAME_SIZE);
    ui.clearAllAnimations();
    return memcmp(immediateFrame, fallbackFrame, FRAME_SIZE) == 0;
#else
    return true;
#endif
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    ui.begin();

    {
        Label widget(ui, 4, 12, "Label", POS::BOTTOM, u8g2_font_4x6_tr);
        if (!verifyLoadTransition(widget, ui, display)) return 1;
    }
    {
        NumScroll widget(ui, 4, 4, 32, 20);
        if (!verifyLoadTransition(widget, ui, display)) return 2;
    }
    {
        TextButton widget(ui, 4, 4, 40, 18, "Button");
        if (!verifyLoadTransition(widget, ui, display)) return 3;
    }
    {
        Brace widget(ui, 4, 4, 40, 20);
        if (!verifyLoadTransition(widget, ui, display)) return 4;
    }
    {
        Clock widget(ui, 32, 32, 20);
        if (!verifyLoadTransition(widget, ui, display)) return 5;
    }

    StaticChartSeries<40> histogramSeries;
    histogramSeries.add(2);
    histogramSeries.add(8);
    {
        Histogram widget(
            ui, 4, 4, 32, 24, histogramSeries,
            ChartExpandSize<40, 30>{}, EXPAND_BASE::TOP_LEFT, "H");
        if (!verifyLoadTransition(widget, ui, display)) return 6;
    }

    StaticChartSeries<40> curveSeries;
    curveSeries.add(2);
    curveSeries.add(8);
    {
        CurveChart widget(
            ui, 4, 4, 32, 24, curveSeries,
            ChartExpandSize<40, 30>{}, EXPAND_BASE::TOP_LEFT, "C");
        if (!verifyLoadTransition(widget, ui, display)) return 7;
    }

    return 0;
}
