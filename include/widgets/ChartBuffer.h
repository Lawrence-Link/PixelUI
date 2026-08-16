#pragma once

#include <stddef.h>
#include <stdint.h>

// Chart samples use signed decimal fixed point with three fractional digits.
// For example, 1.250 is stored as 1250. Keeping the scale in the public API
// makes producer and consumer translation units agree without floating point.
using ChartValue = int32_t;
using ChartAccumulator = int64_t;

inline constexpr ChartValue CHART_VALUE_SCALE = 1000;

constexpr ChartValue chartValueFromInt(int32_t value) {
    return static_cast<ChartValue>(
        static_cast<ChartAccumulator>(value) * CHART_VALUE_SCALE);
}

constexpr ChartValue chartValueFromMilli(int32_t value) {
    return static_cast<ChartValue>(value);
}

template <uint16_t Width, uint16_t Height>
struct ChartExpandSize {
    static_assert(Width > 0U, "Expanded chart width must be greater than zero");
    static_assert(Height > 0U, "Expanded chart height must be greater than zero");

    static constexpr uint16_t width = Width;
    static constexpr uint16_t height = Height;
};
