#include "PixelUI.h"
#include "blink/BlinkState.h"
#include "core/TimeUtils.h"

namespace {

int verifyBasicScheduling() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    int refreshCount = 0;
    ui.setRefreshCallback([&refreshCount]() { ++refreshCount; });

    BlinkState blink(ui, 100U);
    if (!blink.isVisible() || blink.isRunning() || ui.needsHeartbeat()) return 1;

    blink.start();
    if (!blink.isVisible() || !blink.isRunning() || !ui.needsHeartbeat()) return 2;
    if (!ui.renderer() || refreshCount != 1) return 3;
#if PIXELUI_ENABLE_TICKLESS
    if (ui.nextWakeupMs(16U) != 100U) return 10;
#endif

    ui.tickFromISR(99U);
    if (!ui.process() || !blink.isVisible() || ui.renderer()) return 4;
#if PIXELUI_ENABLE_TICKLESS
    if (ui.nextWakeupMs(16U) != 1U) return 11;
#endif
    ui.tickFromISR(1U);
    if (!ui.process() || blink.isVisible() || !ui.renderer()) return 5;
#if PIXELUI_ENABLE_TICKLESS
    if (ui.nextWakeupMs(16U) != 100U) return 12;
#endif

    // Missing two complete periods preserves the final visual state and does
    // not submit an unnecessary frame.
    ui.tickFromISR(200U);
    if (!ui.process() || blink.isVisible() || ui.renderer()) return 6;
    ui.tickFromISR(100U);
    if (!ui.process() || !blink.isVisible() || !ui.renderer()) return 7;

    blink.stop();
    if (blink.isVisible() || blink.isRunning() || !ui.renderer()) return 8;
    if (ui.needsHeartbeat()) return 9;
    return 0;
}

int verifyEarliestDeadline() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    BlinkState fast(ui, 100U);
    BlinkState slow(ui, 250U);

    fast.start();
    slow.start();
    if (!ui.renderer()) return 1;
#if PIXELUI_ENABLE_TICKLESS
    if (ui.nextWakeupMs(16U) != 100U) return 2;
#endif

    ui.tickFromISR(100U);
    ui.process();
    if (fast.isVisible() || !slow.isVisible() || !ui.renderer()) return 3;
#if PIXELUI_ENABLE_TICKLESS
    if (ui.nextWakeupMs(16U) != 100U) return 4;
#endif

    ui.tickFromISR(100U);
    ui.process();
    if (!fast.isVisible() || !slow.isVisible() || !ui.renderer()) return 5;
#if PIXELUI_ENABLE_TICKLESS
    if (ui.nextWakeupMs(16U) != 50U) return 6;
#endif
    return 0;
}

int verifyStopWhenVisibleAndIntervalChange() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    BlinkState blink(ui, 100U);

    blink.start();
    if (!ui.renderer()) return 1;
    ui.tickFromISR(100U);
    ui.process();
    if (blink.isVisible() || !ui.renderer()) return 2;

    blink.stopWhenVisible();
    if (!blink.isRunning()) return 3;
    ui.tickFromISR(100U);
    ui.process();
    if (!blink.isVisible() || blink.isRunning() || !ui.renderer()) return 4;
    if (ui.needsHeartbeat()) return 5;

    blink.start();
    if (!ui.renderer()) return 6;
    blink.setInterval(40U);
    if (blink.interval() != 40U) return 7;
    ui.tickFromISR(39U);
    ui.process();
    if (!blink.isVisible() || ui.renderer()) return 8;
    ui.tickFromISR(1U);
    ui.process();
    if (blink.isVisible() || !ui.renderer()) return 9;

    blink.setInterval(UINT32_MAX);
    if (blink.interval() != PixelUITime::MAX_DEADLINE_INTERVAL) return 10;
    return 0;
}

int verifyLifetimeAndWraparound() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);

    {
        BlinkState temporary(ui, 25U);
        temporary.start();
        if (!ui.renderer() || !ui.needsHeartbeat()) return 1;
    }
    if (ui.needsHeartbeat()) return 2;

    ui.heartbeat(UINT32_MAX - 49U);
    ui.process();
    BlinkState wrapping(ui, 100U);
    wrapping.start();
    if (!ui.renderer()) return 3;
    ui.tickFromISR(99U);
    ui.process();
    if (!wrapping.isVisible() || ui.renderer()) return 4;
    ui.tickFromISR(1U);
    ui.process();
    if (wrapping.isVisible() || !ui.renderer()) return 5;
    return 0;
}

} // namespace

int main() {
    const int basic = verifyBasicScheduling();
    if (basic != 0) return basic;

    const int stop = verifyStopWhenVisibleAndIntervalChange();
    if (stop != 0) return 20 + stop;

    const int earliest = verifyEarliestDeadline();
    if (earliest != 0) return 40 + earliest;

    const int lifetime = verifyLifetimeAndWraparound();
    if (lifetime != 0) return 60 + lifetime;
    return 0;
}
