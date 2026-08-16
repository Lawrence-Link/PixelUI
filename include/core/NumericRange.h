#pragma once

#include <stdint.h>

class NumericRange {
public:
    constexpr NumericRange() = default;

    static constexpr bool tryCreate(
        int32_t minimum,
        int32_t maximum,
        int32_t step,
        NumericRange& output) {
        if (minimum > maximum || step <= 0) return false;
        output.minimum_ = minimum;
        output.maximum_ = maximum;
        output.step_ = step;
        return true;
    }

    constexpr int32_t minimum() const { return minimum_; }
    constexpr int32_t maximum() const { return maximum_; }
    constexpr int32_t step() const { return step_; }

    constexpr int32_t clamp(int32_t value) const {
        return value < minimum_ ? minimum_ :
               value > maximum_ ? maximum_ : value;
    }

    constexpr bool canIncrement(int32_t value) const {
        return clamp(value) < maximum_;
    }

    constexpr bool canDecrement(int32_t value) const {
        return clamp(value) > minimum_;
    }

    constexpr int32_t incremented(int32_t value) const {
        const int32_t current = clamp(value);
        if (current >= maximum_) return maximum_;
        const int64_t candidate = static_cast<int64_t>(current) + step_;
        return candidate >= static_cast<int64_t>(maximum_)
            ? maximum_
            : static_cast<int32_t>(candidate);
    }

    constexpr int32_t decremented(int32_t value) const {
        const int32_t current = clamp(value);
        if (current <= minimum_) return minimum_;
        const int64_t candidate = static_cast<int64_t>(current) - step_;
        return candidate <= static_cast<int64_t>(minimum_)
            ? minimum_
            : static_cast<int32_t>(candidate);
    }

private:
    int32_t minimum_ = 0;
    int32_t maximum_ = 100;
    int32_t step_ = 1;
};

constexpr uint32_t normalizeToExtent(
    const NumericRange& range,
    int32_t value,
    uint32_t extent) {
    const uint64_t span = static_cast<uint64_t>(
        static_cast<int64_t>(range.maximum()) - range.minimum());
    if (span == 0U || extent == 0U) return 0U;

    const int32_t clamped = range.clamp(value);
    const uint64_t offset = static_cast<uint64_t>(
        static_cast<int64_t>(clamped) - range.minimum());
    const uint64_t numerator = offset * extent;
    return static_cast<uint32_t>((numerator + span / 2U) / span);
}
