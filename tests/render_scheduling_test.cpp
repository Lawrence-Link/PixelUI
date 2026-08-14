#include "PixelUI.h"
#include "core/coroutine/Coroutine.h"

namespace {

void countIsrNotification(void* context) {
    ++(*static_cast<int*>(context));
}

#if PIXELUI_ENABLE_TICKLESS
int verifyExactDeadlines() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);

    if (ui.handler(16U) != PixelUI::WAIT_FOREVER) return 1;

    int32_t animatedValue = 0;
    if (!ui.animate(animatedValue, 100, 100U)) return 2;
    if (ui.handler(16U) != 16U) return 3;
    ui.tickFromISR(90U);
    if (ui.handler(16U) != 10U || animatedValue <= 0 || animatedValue >= 100) return 4;
    ui.tickFromISR(10U);
    if (ui.handler(16U) != PixelUI::WAIT_FOREVER || animatedValue != 100) return 5;

    if (!ui.showPopupInfo("Timed", "", 80, 30, 1000U)) return 6;
    if (ui.handler(16U) != 16U) return 7;
    ui.tickFromISR(300U);
    if (ui.handler(16U) != 1000U) return 8;
    ui.tickFromISR(999U);
    if (ui.handler(16U) != 1U) return 9;
    ui.tickFromISR(1U);
    if (ui.handler(16U) != 16U) return 10;
    ui.tickFromISR(300U);
    if (ui.handler(16U) != PixelUI::WAIT_FOREVER || ui.popupCount() != 0U) return 11;

    int delayStage = 0;
    Coroutine delayed([&ui, &delayStage](CoroutineContext& ctx) {
        CORO_BEGIN(ctx);
        ++delayStage;
        CORO_DELAY(ctx, ui, 100U, 1);
        ++delayStage;
        CORO_END(ctx);
    });
    delayed.start();
    ui.addCoroutine(&delayed);
    if (ui.handler(16U) != 100U || delayStage != 1) return 12;
    ui.tickFromISR(99U);
    if (ui.handler(16U) != 1U || delayStage != 1) return 13;
    ui.tickFromISR(1U);
    if (ui.handler(16U) != PixelUI::WAIT_FOREVER || delayStage != 2) return 14;

    int waitStage = 0;
    int32_t waitValue = 0;
    Coroutine waitsForAnimation([&ui, &waitStage](CoroutineContext& ctx) {
        CORO_BEGIN(ctx);
        CORO_WAIT_ANIMATION(ctx, ui, 2);
        ++waitStage;
        CORO_END(ctx);
    });
    waitsForAnimation.start();
    ui.addCoroutine(&waitsForAnimation);
    if (!ui.animate(waitValue, 10, 100U)) return 15;
    if (ui.handler(16U) != 16U || waitStage != 0) return 16;
    ui.tickFromISR(100U);
    if (ui.handler(16U) != 0U || waitValue != 10 || waitStage != 0) return 17;
    if (ui.handler(16U) != PixelUI::WAIT_FOREVER || waitStage != 1) return 18;

    return 0;
}
#endif

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

#if PIXELUI_ENABLE_TICKLESS
    const int deadlineResult = verifyExactDeadlines();
    if (deadlineResult != 0) return 100 + deadlineResult;
#else
    if (ui.handler(25U) != 25U) return 25;
#endif

    return 0;
}
