#include "PixelUI.h"
#include "core/NumericFormatter.h"
#include "core/NumericRange.h"
#include "core/ValueEdit.h"
#include "ui/Popup/PopupProgress.h"

namespace {

struct ChangeState {
    int calls = 0;
    int32_t last = 0;
};

struct BindingState {
    int32_t value = 0;
    mutable int reads = 0;
    int writes = 0;
};

bool readValue(const void* context, int32_t& value) {
    const BindingState& state = *static_cast<const BindingState*>(context);
    ++state.reads;
    value = state.value;
    return true;
}

bool writeValue(void* context, int32_t value) {
    BindingState& state = *static_cast<BindingState*>(context);
    ++state.writes;
    state.value = value;
    return true;
}

void changed(void* context, int32_t value) {
    ChangeState& state = *static_cast<ChangeState*>(context);
    ++state.calls;
    state.last = value;
}

void show(PopupProgress& popup, PixelUI& ui) {
    popup.update(ui.getCurrentTime());
    ui.heartbeat(300U);
    popup.update(ui.getCurrentTime());
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);

    NumericRange range;
    if (!NumericRange::tryCreate(INT32_MIN, INT32_MAX, 1000000000, range)) return 1;

    int32_t value = INT32_MIN;
    ChangeState state;
    ValueEditSession session(
        ValueEditorBinding::reference(value, &changed, &state),
        ValueEditPolicy::CommitOnConfirm);
    PopupProgress popup(
        ui, 100, 40, range, NumericFormatter::integer(), session, "Value", 0,
        PopupProgressMode::Editable);
    show(popup, ui);
    if (!popup.handleInput(InputEvent::RIGHT) || value != INT32_MIN ||
        session.draftValue() != -1147483648 || state.calls != 0) return 2;
    if (!popup.handleInput(InputEvent::SELECT) || value != -1147483648 ||
        state.calls != 1 || state.last != value) return 3;

    int32_t cancelled = INT32_MAX;
    ChangeState cancelState;
    ValueEditSession cancelSession(
        ValueEditorBinding::reference(cancelled, &changed, &cancelState),
        ValueEditPolicy::CommitOnConfirm);
    PopupProgress cancelPopup(
        ui, 100, 40, range, NumericFormatter::integer(), cancelSession, "", 0,
        PopupProgressMode::Editable);
    show(cancelPopup, ui);
    cancelPopup.handleInput(InputEvent::LEFT);
    cancelPopup.handleInput(InputEvent::BACK);
    if (cancelled != INT32_MAX || cancelState.calls != 0 ||
        cancelSession.draftValue() != INT32_MAX) return 4;

    int32_t liveValue = 0;
    ChangeState liveState;
    ValueEditSession liveSession(
        ValueEditorBinding::reference(liveValue, &changed, &liveState),
        ValueEditPolicy::Live);
    PopupProgress livePopup(
        ui, 100, 40, range, NumericFormatter::integer(), liveSession, "", 0,
        PopupProgressMode::Editable);
    show(livePopup, ui);
    livePopup.handleInput(InputEvent::RIGHT);
    if (liveValue != 1000000000 || liveState.calls != 1) return 5;
    livePopup.handleInput(InputEvent::BACK);
    if (liveValue != 0 || liveState.calls != 2 || liveState.last != 0) return 6;

    BindingState readOnlyValue{10};
    ChangeState readOnlyState;
    PopupProgress readOnlyPopup(
        ui, 100, 40, range, NumericFormatter::integer(),
        ValueEditorBinding::custom(
            &readOnlyValue, &readValue, &writeValue, &changed, &readOnlyState),
        "Read only", 0, nullptr, ValueEditPolicy::Live);
    show(readOnlyPopup, ui);
    if (!readOnlyPopup.handleInput(InputEvent::RIGHT) ||
        readOnlyValue.value != 10 || readOnlyValue.writes != 0 ||
        readOnlyState.calls != 0) return 7;
    if (!readOnlyPopup.handleInput(InputEvent::LEFT) ||
        readOnlyValue.value != 10 || readOnlyValue.writes != 0 ||
        readOnlyState.calls != 0) return 8;
    readOnlyValue.value = 20;
    const int readsBeforeDraw = readOnlyValue.reads;
    readOnlyPopup.drawContent(PopupContentBounds{0, 0, 100, 40, 50, 20});
    if (readOnlyValue.reads != readsBeforeDraw + 1 ||
        readOnlyValue.writes != 0 || readOnlyState.calls != 0) return 9;
    if (!readOnlyPopup.handleInput(InputEvent::SELECT) ||
        readOnlyValue.value != 20 || readOnlyValue.writes != 0 ||
        readOnlyState.calls != 0) return 10;

    return 0;
}
