#include "PixelUI.h"
#include "core/animation/animation.h"
#include "ui/Popup/PopupValueDigits.h"
#include <string.h>

namespace {

struct ChangeState {
    int calls = 0;
    int32_t last = 0;
};

void changed(void* context, int32_t value) {
    ChangeState& state = *static_cast<ChangeState*>(context);
    ++state.calls;
    state.last = value;
}

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
        ui, 100, 56, value, 4, "Value", 0,
        [&callbackCount, &callbackValue](int32_t updated) {
            ++callbackCount;
            callbackValue = updated;
        });

    popup.update(ui.getCurrentTime());
    ui.heartbeat(100);
    popup.update(ui.getCurrentTime());
    display.clearBuffer();
    popup.draw();

    constexpr int32_t fixedPointOne = 1 << 10;
    const int32_t progress = 100 * fixedPointOne / 300;
    const int32_t eased = EasingCalculator::calculate(EasingType::EASE_OUT_CUBIC, progress);
    const int32_t currentWidth = static_cast<int32_t>(
        (static_cast<int64_t>(100 << 10) * eased) / fixedPointOne) >> 10;
    const int32_t currentHeight = currentWidth * 56 / 100;
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

    ui.heartbeat(200);
    popup.update(ui.getCurrentTime());

    uint8_t unscrolledFrame[1024] = {};
    display.clearBuffer();
    popup.draw();
    if (!pixelIsSet(display, 35, 43) || !pixelIsSet(display, 65, 43)) return 2;
    memcpy(unscrolledFrame, u8g2_GetBufferPtr(display.getU8g2()),
           ui.getDisplayBufferSize());

    ui.getCanvas().camera().setEnabled(true);
    ui.getCanvas().camera().setContentHeight(128);
    ui.getCanvas().camera().setY(32);
    display.clearBuffer();
    popup.draw();
    if (memcmp(unscrolledFrame, u8g2_GetBufferPtr(display.getU8g2()),
               ui.getDisplayBufferSize()) != 0) return 3;

    // Editing SELECT only leaves the current digit. It must not commit or close,
    // so another digit can be edited before the OK action is selected.
    popup.handleInput(InputEvent::RIGHT);  // focus thousands
    popup.handleInput(InputEvent::SELECT);
    popup.handleInput(InputEvent::RIGHT);  // 0 -> 1
    popup.handleInput(InputEvent::SELECT); // finish this digit only
    if (value != 123 || callbackCount != 0) return 4;

    popup.handleInput(InputEvent::RIGHT);  // hundreds
    popup.handleInput(InputEvent::RIGHT);  // tens
    popup.handleInput(InputEvent::SELECT);
    popup.handleInput(InputEvent::RIGHT);  // 2 -> 3
    popup.handleInput(InputEvent::SELECT); // finish this digit only
    if (value != 123 || callbackCount != 0) return 5;

    popup.handleInput(InputEvent::RIGHT);  // ones
    popup.handleInput(InputEvent::RIGHT);  // OK
    popup.handleInput(InputEvent::SELECT);
    if (value != 1133 || callbackCount != 1 || callbackValue != 1133) return 6;

    int32_t clamped = 1234567;
    PopupValueDigits sixDigits(ui, 100, 56, clamped, 6, "", 0);
    if (clamped != 1234567) return 6;
    sixDigits.handleInput(InputEvent::BACK);
    if (clamped != 1234567) return 7;

    int cancelCallbackCount = 0;
    int32_t cancelled = 123;
    PopupValueDigits cancelledPopup(
        ui, 100, 56, cancelled, 4, "", 0,
        [&cancelCallbackCount](int32_t) { ++cancelCallbackCount; });
    cancelledPopup.handleInput(InputEvent::RIGHT);  // first digit
    cancelledPopup.handleInput(InputEvent::SELECT);
    cancelledPopup.handleInput(InputEvent::RIGHT);
    cancelledPopup.handleInput(InputEvent::SELECT); // finish digit
    cancelledPopup.handleInput(InputEvent::LEFT);   // wrap to CANCEL
    cancelledPopup.handleInput(InputEvent::SELECT);
    if (cancelled != 123 || cancelCallbackCount != 0) return 8;

    if (PopupValueDigits::isValidDigitCount(0) ||
        PopupValueDigits::isValidDigitCount(MAX_INT_FIXED_WIDTH + 1U)) return 9;

    ui.clearAllAnimations();
    int32_t liveValue = 123;
    ChangeState liveState;
    int liveCompatibilityCalls = 0;
    PopupValueDigits livePopup(
        ui, 100, 56,
        ValueEditorBinding::reference(liveValue, &changed, &liveState),
        4, "", 0,
        [&liveCompatibilityCalls](int32_t) { ++liveCompatibilityCalls; },
        ValueEditPolicy::Live);
    if (liveValue != 123 || liveState.calls != 0 || liveCompatibilityCalls != 0) {
        return 10;
    }
    livePopup.handleInput(InputEvent::RIGHT);  // first digit
    livePopup.handleInput(InputEvent::SELECT);
    livePopup.handleInput(InputEvent::RIGHT);
    if (liveValue != 1123 || liveState.calls != 1 || liveState.last != 1123 ||
        liveCompatibilityCalls != 1) return 11;
    livePopup.handleInput(InputEvent::SELECT); // finish digit, do not commit
    livePopup.handleInput(InputEvent::LEFT);   // CANCEL
    livePopup.handleInput(InputEvent::SELECT);
    if (liveValue != 123 || liveState.calls != 2 || liveState.last != 123 ||
        liveCompatibilityCalls != 2) return 12;

    int32_t timeoutValue = 123;
    ChangeState timeoutState;
    {
        PopupValueDigits timeoutPopup(
            ui, 100, 56,
            ValueEditorBinding::reference(timeoutValue, &changed, &timeoutState),
            4, "", 100, nullptr, ValueEditPolicy::Live);
        timeoutPopup.update(0);
        timeoutPopup.update(300);
        timeoutPopup.handleInput(InputEvent::RIGHT);
        timeoutPopup.handleInput(InputEvent::SELECT);
        timeoutPopup.handleInput(InputEvent::RIGHT);
        if (timeoutValue != 1123 || timeoutState.calls != 1) return 13;
        timeoutPopup.update(401);
        if (timeoutValue != 123 || timeoutState.calls != 2 ||
            timeoutState.last != 123) return 14;
    }

    int32_t destroyedValue = 123;
    ChangeState destroyedState;
    {
        PopupValueDigits destroyedPopup(
            ui, 100, 56,
            ValueEditorBinding::reference(
                destroyedValue, &changed, &destroyedState),
            4, "", 0, nullptr, ValueEditPolicy::Live);
        destroyedPopup.handleInput(InputEvent::RIGHT);
        destroyedPopup.handleInput(InputEvent::SELECT);
        destroyedPopup.handleInput(InputEvent::RIGHT);
        if (destroyedValue != 1123 || destroyedState.calls != 1) return 15;
    }
    if (destroyedValue != 123 || destroyedState.calls != 2 ||
        destroyedState.last != 123) return 16;

    ui.clearAllAnimations();
    int32_t occupiedAnimations[MAX_ANIMATION_COUNT]{};
    for (int i = 0; i < MAX_ANIMATION_COUNT; ++i) {
        if (!ui.animate(occupiedAnimations[i], 1, 1000)) return 17;
    }
    int32_t fullAnimationValue = 123;
    PopupValueDigits fullAnimationPopup(
        ui, 100, 56, fullAnimationValue, 4, "", 0);
    fullAnimationPopup.handleInput(InputEvent::RIGHT);
    fullAnimationPopup.handleInput(InputEvent::SELECT);
    fullAnimationPopup.handleInput(InputEvent::RIGHT);
    fullAnimationPopup.handleInput(InputEvent::SELECT);
    fullAnimationPopup.handleInput(InputEvent::RIGHT);
    fullAnimationPopup.handleInput(InputEvent::RIGHT);
    fullAnimationPopup.handleInput(InputEvent::RIGHT);
    fullAnimationPopup.handleInput(InputEvent::RIGHT);
    fullAnimationPopup.handleInput(InputEvent::SELECT);
    if (fullAnimationValue != 1123) return 18;
    ui.clearAllAnimations();

    return 0;
}
