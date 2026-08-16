#include "PixelUI.h"
#include "ui/Popup/PopupManager.h"

namespace {

void advance(PopupManager& manager, PixelUI& ui, uint32_t milliseconds) {
    ui.heartbeat(milliseconds);
    manager.updatePopups(ui.getCurrentTime());
}

void finishActive(PopupManager& manager, PixelUI& ui) {
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    manager.handleTopPopupInput(InputEvent::BACK);
    advance(manager, ui, 300U);
}

} // namespace

int main() {
    U8G2 display;
    PixelUI ui(display);
    PopupManager manager(ui);
    int32_t firstValue = 10;
    int32_t secondValue = 20;

    // The first request becomes active; later requests remain FIFO pending.
    if (!manager.enqueueProgress(80, 30, firstValue, 0, 100, "First", 0)) return 1;
    if (!manager.enqueueProgress(80, 30, secondValue, 0, 100, "Second", 0)) return 2;
    if (!manager.enqueueInfo(80, 30, "Third", "", 0)) return 3;
    if (!manager.hasActivePopup() || (manager.pendingCount() != 2U) ||
        (manager.getPopupCounts() != MAX_POPUP_NUM)) return 4;

    // Capacity includes the active Popup. Full queues reject without eviction.
    if (manager.enqueueInfo(80, 30, "Rejected", "", 0)) return 5;
    if ((manager.getPopupCounts() != MAX_POPUP_NUM) || (manager.pendingCount() != 2U)) return 6;

    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    if (!manager.handleTopPopupInput(InputEvent::RIGHT) || (firstValue != 10) ||
        (secondValue != 20)) return 7;
    if (!manager.handleTopPopupInput(InputEvent::SELECT) || firstValue != 11) return 8;

    // Closing the first active Popup activates the second request immediately.
    advance(manager, ui, 300U);
    if (!manager.hasActivePopup() || (manager.pendingCount() != 1U) ||
        (manager.getPopupCounts() != 2U)) return 9;
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    if (!manager.handleTopPopupInput(InputEvent::RIGHT) || (secondValue != 20)) return 10;
    manager.handleTopPopupInput(InputEvent::BACK);
    if (secondValue != 20) return 11;

    advance(manager, ui, 300U);
    if (!manager.hasActivePopup() || (manager.pendingCount() != 0U) ||
        (manager.getPopupCounts() != 1U)) return 12;
    finishActive(manager, ui);
    if (manager.hasActivePopup() || (manager.getPopupCounts() != 0U)) return 13;

    // clearPopups destroys the active object and drops every pending request.
    if (!manager.enqueueInfo(80, 30, "Active", "", 0) ||
        !manager.enqueueInfo(80, 30, "Pending", "", 0)) return 14;
    manager.clearPopups();
    if (manager.hasActivePopup() || (manager.pendingCount() != 0U) ||
        (manager.getPopupCounts() != 0U)) return 15;

    // Structural mutation from a Popup callback is rejected while dispatching.
    bool callbackRan = false;
    ValueCallback callback = [&manager, &callbackRan](int32_t) {
        callbackRan = true;
        manager.clearPopups();
    };
    if (!manager.enqueueProgress(
            80, 30, firstValue, 0, 100, "Reentrant", 0,
            etl::move(callback))) return 16;
    if (callback) return 17;
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    if (!manager.handleTopPopupInput(InputEvent::RIGHT) || callbackRan) return 18;
    if (!manager.handleTopPopupInput(InputEvent::SELECT) || !callbackRan ||
        (manager.getPopupCounts() != 1U)) return 19;
    manager.clearPopups();

    // Destroying PopupValueDigits invokes its derived cleanup before reuse.
    int32_t animatedValue = 0;
    if (!ui.animate(animatedValue, 10, 1000)) return 20;
    if (!manager.enqueueValueDigits(80, 40, firstValue, 4, "Digits", 0)) return 21;
    manager.clearPopups();
    if ((ui.activeAnimationCount() != 1U) || manager.hasActivePopup()) return 22;
    ui.clearAllAnimations();

    // Timed completion also advances the FIFO.
    if (!manager.enqueueInfo(80, 30, "Timed", "", 1) ||
        !manager.enqueueInfo(80, 30, "Next", "", 0)) return 23;
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    advance(manager, ui, 1U);
    if ((manager.getPopupCounts() != 2U) || !manager.hasActivePopup()) return 24;
    advance(manager, ui, 300U);
    if ((manager.getPopupCounts() != 1U) || !manager.hasActivePopup() ||
        (manager.pendingCount() != 0U)) return 25;
    manager.clearPopups();

    // A mixed Info -> Progress -> ValueDigits sequence preserves strict FIFO.
    int32_t mixedProgress = 7;
    int32_t mixedDigits = 42;
    int progressCallbackCount = 0;
    int digitsCallbackCount = 0;
    ValueCallback progressCallback = [&progressCallbackCount](int32_t) {
        ++progressCallbackCount;
    };
    ValueCallback digitsCallback = [&digitsCallbackCount](int32_t) {
        ++digitsCallbackCount;
    };
    if (!manager.enqueueInfo(80, 30, "Info", "", 0) ||
        !manager.enqueueProgress(
            80, 30, mixedProgress, 0, 10, "Progress", 0,
            etl::move(progressCallback)) ||
        !manager.enqueueValueDigits(
            80, 40, mixedDigits, 4, "Digits", 0,
            etl::move(digitsCallback))) return 26;
    if (progressCallback || digitsCallback || manager.getPopupCounts() != 3U) {
        return 27;
    }

    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    manager.handleTopPopupInput(InputEvent::BACK);
    advance(manager, ui, 300U);
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    manager.handleTopPopupInput(InputEvent::RIGHT);
    if (mixedProgress != 7 || mixedDigits != 42) return 28;
    manager.handleTopPopupInput(InputEvent::SELECT);
    if (mixedProgress != 8 || progressCallbackCount != 1 ||
        mixedDigits != 42 || digitsCallbackCount != 0) return 29;

    advance(manager, ui, 300U);
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    manager.handleTopPopupInput(InputEvent::RIGHT);
    manager.handleTopPopupInput(InputEvent::SELECT);
    manager.handleTopPopupInput(InputEvent::RIGHT);
    manager.handleTopPopupInput(InputEvent::SELECT);
    if (mixedDigits != 1042 || digitsCallbackCount != 1 ||
        manager.pendingCount() != 0U) return 30;

    return 0;
}
