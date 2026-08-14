#include "PixelUI.h"

namespace {

void countIsrNotification(void* context) {
    ++(*static_cast<int*>(context));
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    int refreshCount = 0;
    int renderRequestCount = 0;
    int taskNotifyCount = 0;
    ui.setRefreshCallback([&refreshCount]() { ++refreshCount; });
    ui.setRenderRequestCallback([&renderRequestCount]() { ++renderRequestCount; });
    ui.setTaskNotifyFromISR(countIsrNotification, &taskNotifyCount);

    // The ISR path only accumulates time and coalesces task notifications.
    if (ui.needsHeartbeat() || ui.hasPendingFrame() || ui.renderer()) return 1;
    ui.tickFromISR(7U);
    ui.tickFromISR(9U);
    if (taskNotifyCount != 1 || ui.getCurrentTime() != 16U) return 2;
    if (!ui.process() || ui.getCurrentTime() != 16U || ui.process()) return 3;
    if (ui.hasPendingFrame() || ui.renderer() || refreshCount != 0) return 4;

    // Once process() drains a batch, a later ISR tick must retain a new wakeup.
    ui.tickFromISR(16U);
    if (taskNotifyCount != 2 || !ui.process()) return 5;

#if PIXELUI_ENABLE_TICKLESS
    if (ui.nextWakeupMs(16U) != PixelUI::WAIT_FOREVER) return 6;
#else
    if (ui.nextWakeupMs(16U) != 16U || ui.nextWakeupMs(0U) != 1U) return 6;
#endif

    // Multiple render passes consume one explicit invalidation only once.
    ui.markDirty();
    ui.markDirty();
    if (renderRequestCount != 1) return 7;
    if (!ui.hasPendingFrame() || !ui.renderer() || refreshCount != 1) return 8;
    if (ui.hasPendingFrame() || ui.renderer() || refreshCount != 1) return 9;

    // Animation updates submit changed values, including the completion value.
    int32_t value = 0;
    if (!ui.animate(value, 100, 100U) || !ui.needsHeartbeat()) return 10;
    if (renderRequestCount != 2 || !ui.renderer() || refreshCount != 2) return 11;
    ui.tickFromISR(50U);
    if (value != 0 || taskNotifyCount != 3) return 12;
    if (!ui.process() || value <= 0 || value >= 100) return 13;
    if (!ui.renderer() || refreshCount != 3) return 14;
    ui.tickFromISR(50U);
    if (!ui.process() || !ui.renderer() || value != 100 || refreshCount != 4) return 15;
    if (ui.needsHeartbeat()) return 16;
    ui.Heartbeat(16U);
    if (ui.renderer() || refreshCount != 4) return 17;

    // Continuous mode is an explicit opt-in to one frame per render pass.
    ui.setContinuousDraw(true);
    if (!ui.needsHeartbeat() || !ui.renderer() || !ui.renderer() || refreshCount != 6) return 18;
    ui.setContinuousDraw(false);
    if (ui.needsHeartbeat() || ui.renderer() || refreshCount != 6) return 19;

    // Fade remains time-driven, but submits only when its 40 ms step is due.
    ui.markFading();
    if (!ui.needsHeartbeat()) return 20;
    if (ui.renderer() || refreshCount != 6) return 21;
    for (int step = 0; step < 4; ++step) {
        ui.tickFromISR(40U);
        if (!ui.process() || !ui.renderer()) return 22;
    }
    if (ui.needsHeartbeat() || ui.hasPendingFrame() || ui.renderer()) return 23;
    if (refreshCount != 10) return 24;

    return 0;
}
