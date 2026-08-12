#pragma once

#include <stddef.h>
#include <stdint.h>

template <uint16_t Width, uint16_t Height>
struct ChartExpandSize {
    static_assert(Width > 0U, "Expanded chart width must be greater than zero");
    static_assert(Height > 0U, "Expanded chart height must be greater than zero");

    static constexpr uint16_t width = Width;
    static constexpr uint16_t height = Height;
};

