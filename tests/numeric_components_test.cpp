#include "core/NumericFormatter.h"
#include "core/NumericRange.h"
#include "core/ValueEdit.h"

#include <stdint.h>
#include <string.h>

namespace {

struct ChangeState {
    int32_t value = 0;
    int32_t last = 0;
    int calls = 0;
};

void changed(void* context, int32_t value) {
    ChangeState& state = *static_cast<ChangeState*>(context);
    state.last = value;
    ++state.calls;
}

bool matches(const NumericFormatter& formatter, int32_t value, const char* expected) {
    char buffer[32]{};
    return formatter.format(value, buffer, sizeof(buffer)) &&
           strcmp(buffer, expected) == 0;
}

} // namespace

int main() {
    NumericRange range;
    if (NumericRange::tryCreate(10, 9, 1, range)) return 1;
    if (NumericRange::tryCreate(0, 10, 0, range)) return 2;
    if (!NumericRange::tryCreate(0, 10, 3, range)) return 3;
    if (range.incremented(9) != 10 || range.incremented(10) != 10 ||
        range.decremented(1) != 0 || range.decremented(0) != 0) return 4;
    if (!range.canIncrement(9) || range.canIncrement(10) ||
        !range.canDecrement(1) || range.canDecrement(0)) return 5;

    NumericRange extremes;
    if (!NumericRange::tryCreate(INT32_MIN, INT32_MAX, INT32_MAX, extremes)) return 6;
    if (extremes.incremented(INT32_MIN) != -1 ||
        extremes.incremented(-1) != INT32_MAX - 1 ||
        extremes.incremented(INT32_MAX - 1) != INT32_MAX ||
        extremes.decremented(INT32_MAX) != 0 ||
        extremes.decremented(INT32_MIN + 1) != INT32_MIN) return 7;
    if (normalizeToExtent(extremes, INT32_MIN, 97U) != 0U ||
        normalizeToExtent(extremes, INT32_MAX, 97U) != 97U ||
        normalizeToExtent(extremes, 0, 100U) != 50U) return 8;

    if (!matches(NumericFormatter::integer(), INT32_MIN, "-2147483648")) return 9;
    const IntegerFormat padded{3U, nullptr};
    if (!matches(NumericFormatter::integer(padded), -7, "-007")) return 10;

    const ScaledIntegerFormat scaled{10U, 1U, 1U, " V"};
    if (!matches(NumericFormatter::scaled(scaled), -7, "-0.7 V")) return 11;
    const ScaledIntegerFormat rounded{1000U, 2U, 1U, nullptr};
    if (!matches(NumericFormatter::scaled(rounded), 1999, "2.00")) return 12;

    NumericRange percentRange;
    if (!NumericRange::tryCreate(-50, 150, 1, percentRange)) return 13;
    const PercentageFormat percentage{&percentRange, "%"};
    const NumericFormatter percentFormatter = NumericFormatter::percentage(percentage);
    if (!matches(percentFormatter, -50, "0%") ||
        !matches(percentFormatter, 50, "50%") ||
        !matches(percentFormatter, 150, "100%")) return 14;

    char exact[5]{};
    if (!NumericFormatter::integer(padded).format(-7, exact, sizeof(exact)) ||
        strcmp(exact, "-007") != 0) return 15;
    char shortBuffer[4] = {'x', 'x', 'x', '\0'};
    if (NumericFormatter::integer(padded).format(-7, shortBuffer, sizeof(shortBuffer)) ||
        shortBuffer[0] != '\0') return 16;

    char composite[24]{};
    FixedBufferWriter writer(composite, sizeof(composite));
    if (!writer.appendInteger(INT32_MIN) || !writer.append("/") ||
        !writer.appendInteger(INT32_MAX) || !writer.finish() ||
        strcmp(composite, "-2147483648/2147483647") != 0) return 17;

    ChangeState liveState{10, 0, 0};
    ValueEditSession live(
        ValueEditorBinding::reference(liveState.value, &changed, &liveState),
        ValueEditPolicy::Live);
    if (!live.setDraftValue(12) || liveState.value != 12 || liveState.calls != 1 ||
        liveState.last != 12) return 18;
    if (!live.cancel() || liveState.value != 10 || liveState.calls != 2 ||
        liveState.last != 10) return 19;

    ChangeState deferredState{20, 0, 0};
    ValueEditSession deferred(
        ValueEditorBinding::reference(
            deferredState.value, &changed, &deferredState),
        ValueEditPolicy::CommitOnConfirm);
    if (!deferred.setDraftValue(25) || deferredState.value != 20 ||
        deferredState.calls != 0) return 20;
    if (!deferred.cancel() || deferredState.value != 20 ||
        deferred.draftValue() != 20 || deferredState.calls != 0) return 21;
    if (!deferred.setDraftValue(30) || !deferred.commit() ||
        deferredState.value != 30 || deferredState.calls != 1 ||
        deferredState.last != 30) return 22;

    return 0;
}
