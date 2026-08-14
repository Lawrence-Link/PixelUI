# Event-driven rendering

PixelUI separates three kinds of work:

- An external event mutates UI state and calls `markDirty()`.
- A timer ISR calls `tickFromISR(elapsedMs)` with the host-selected interval.
- The UI task calls `process()` to consume time and update managers.
- `renderer()` consumes pending work and returns true only when it submits a frame.

`markDirty()` coalesces repeated invalidations. `setRenderRequestCallback()` is
called only when the dirty state changes from clean to dirty, so an RTOS task or
platform event loop can use it as a wake-up notification without building an
unbounded event queue. The callback must post or signal work; it must not call
`renderer()` synchronously from inside the state mutation that raised it.

```cpp
void notifyUiTaskFromISR(void* taskHandle) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(
        static_cast<TaskHandle_t>(taskHandle),
        &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

ui.setTaskNotifyFromISR(notifyUiTaskFromISR, uiTaskHandle);

for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    dispatchInputAndDataEvents();
    ui.process();
    ui.renderer();
}
```

`tickFromISR()` performs only an atomic millisecond accumulation and a
coalesced task notification. All PixelUI state mutation, `process()`, and
`renderer()` calls remain single-thread-affine. An interrupt or producer task
should enqueue its domain event and wake the UI task instead of calling widget
or manager APIs directly.

The embedded target must provide an ISR-safe, native 32-bit atomic operation
for `etl::atomic<uint32_t>`. Configure `setTaskNotifyFromISR()` before enabling
the timer interrupt, and do not replace its callback or context while that
interrupt can run. When task notification is the wake-up mechanism, inject
time through `tickFromISR()` only; mixing it with `Heartbeat()` can accumulate
time without producing the notification that the blocked UI task expects.

The legacy `Heartbeat(ms)` API remains available for non-ISR hosts. It only
accumulates time; manager updates still happen in `process()` or, for backward
compatibility, at the beginning of `renderer()`.

## Timer policy

The normal embedded configuration keeps a periodic Timer ISR. Its interval is
not fixed by PixelUI: pass the actual elapsed milliseconds to `tickFromISR()`.

`PIXELUI_ENABLE_TICKLESS` defaults to `0`. When enabled, the host may use
`nextWakeupMs(periodicTickMs)` after processing an event:

- A finite result requests the next periodic animation/fade tick.
- `PixelUI::WAIT_FOREVER` means no time-driven UI work is active, so the host
  may stop its UI timer until an input or data event starts new work.

This first tickless policy suppresses only completely idle ticks. It does not
yet calculate exact Popup or Coroutine deadlines.

## Refactoring plan

1. Render invalidation and idle gating (implemented)
   - Make `markDirty()` the coalescing frame-request boundary.
   - Skip buffer clearing, drawing, and `sendBuffer()` while clean.
   - Preserve animation completion frames, Popup transitions, continuous mode,
     and fade progress.
   - Expose `needsHeartbeat()`, `hasPendingFrame()`, and the render-request
     callback for host integration.

2. ISR/UI-task separation and optional idle tick suppression (implemented)
   - Keep ISR work to atomic tick accumulation and task notification.
   - Update every manager from `process()` on the UI task.
   - Keep periodic timers by default and expose opt-in tickless policy.

3. Deadline-based timers
   - Give AnimationManager, PopupManager, and CoroutineScheduler a
     `nextDeadline()` query.
   - Replace a fixed-rate Heartbeat timer with a one-shot timer for the earliest
     deadline.
   - Keep a configurable frame cadence only while smooth animation or explicit
     continuous drawing is active.

4. Domain event queue
   - Define fixed-capacity input, data-change, navigation, and timer events.
   - Drain the queue on the UI thread before updating managers and rendering.
   - Define overflow and event-coalescing policies explicitly; never mutate UI
     ownership or widget state from an ISR.

5. Dirty-region rendering (optional)
   - Add invalidated rectangles only after full-frame scheduling is stable.
   - Use partial display updates only on controllers where they are correct and
     measurably beneficial; retain full-buffer composition as the fallback.

Acceptance criteria for each phase are: no idle `sendBuffer()`, no lost final
animation/fade frame, deterministic fixed-capacity failure behavior, and normal
plus sanitizer lifecycle tests passing.
