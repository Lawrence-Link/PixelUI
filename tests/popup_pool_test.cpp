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
    if (!manager.handleTopPopupInput(InputEvent::RIGHT) || (firstValue != 11) ||
        (secondValue != 20)) return 7;

    // Closing the first active Popup activates the second request immediately.
    finishActive(manager, ui);
    if (!manager.hasActivePopup() || (manager.pendingCount() != 1U) ||
        (manager.getPopupCounts() != 2U)) return 8;
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    if (!manager.handleTopPopupInput(InputEvent::RIGHT) || (secondValue != 21)) return 9;

    finishActive(manager, ui);
    if (!manager.hasActivePopup() || (manager.pendingCount() != 0U) ||
        (manager.getPopupCounts() != 1U)) return 10;
    finishActive(manager, ui);
    if (manager.hasActivePopup() || (manager.getPopupCounts() != 0U)) return 11;

    // clearPopups destroys the active object and drops every pending request.
    if (!manager.enqueueInfo(80, 30, "Active", "", 0) ||
        !manager.enqueueInfo(80, 30, "Pending", "", 0)) return 12;
    manager.clearPopups();
    if (manager.hasActivePopup() || (manager.pendingCount() != 0U) ||
        (manager.getPopupCounts() != 0U)) return 13;

    // Structural mutation from a Popup callback is rejected while dispatching.
    bool callbackRan = false;
    ValueCallback callback = [&manager, &callbackRan](int32_t) {
        callbackRan = true;
        manager.clearPopups();
    };
    if (!manager.enqueueProgress(
            80, 30, firstValue, 0, 100, "Reentrant", 0,
            etl::move(callback))) return 14;
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    if (!manager.handleTopPopupInput(InputEvent::RIGHT) || !callbackRan ||
        (manager.getPopupCounts() != 1U)) return 15;
    manager.clearPopups();

    // Destroying PopupValueDigits invokes its derived cleanup before reuse.
    int32_t animatedValue = 0;
    if (!ui.animate(animatedValue, 10, 1000)) return 16;
    if (!manager.enqueueValueDigits(80, 40, firstValue, 4, "Digits", 0)) return 17;
    manager.clearPopups();
    if ((ui.activeAnimationCount() != 0U) || manager.hasActivePopup()) return 18;

    // Timed completion also advances the FIFO.
    if (!manager.enqueueInfo(80, 30, "Timed", "", 1) ||
        !manager.enqueueInfo(80, 30, "Next", "", 0)) return 19;
    manager.updatePopups(ui.getCurrentTime());
    advance(manager, ui, 300U);
    advance(manager, ui, 1U);
    if ((manager.getPopupCounts() != 2U) || !manager.hasActivePopup()) return 20;
    advance(manager, ui, 300U);
    if ((manager.getPopupCounts() != 1U) || !manager.hasActivePopup() ||
        (manager.pendingCount() != 0U)) return 21;

    return 0;
}
