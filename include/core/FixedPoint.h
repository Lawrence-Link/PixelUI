#pragma once

#include <stddef.h>
#include <stdint.h>

namespace PixelUIFixedPoint {

// Formats a signed value stored in tenths without pulling floating-point printf
// support into the firmware. For example, -123 becomes "-12.3".
inline bool formatDecimal1(char* buffer, size_t bufferSize, int32_t tenths) {
    if (buffer == nullptr || bufferSize == 0U) return false;

    const bool negative = tenths < 0;
    const uint32_t magnitude = negative
        ? 0U - static_cast<uint32_t>(tenths)
        : static_cast<uint32_t>(tenths);
    uint32_t whole = magnitude / 10U;
    const char fractional = static_cast<char>('0' + (magnitude % 10U));

    char reversed[10];
    size_t digitCount = 0U;
    do {
        reversed[digitCount++] = static_cast<char>('0' + (whole % 10U));
        whole /= 10U;
    } while (whole != 0U);

    const size_t required = (negative ? 1U : 0U) + digitCount + 3U;
    if (bufferSize < required) {
        buffer[0] = '\0';
        return false;
    }

    size_t output = 0U;
    if (negative) buffer[output++] = '-';
    while (digitCount != 0U) buffer[output++] = reversed[--digitCount];
    buffer[output++] = '.';
    buffer[output++] = fractional;
    buffer[output] = '\0';
    return true;
}

} // namespace PixelUIFixedPoint
