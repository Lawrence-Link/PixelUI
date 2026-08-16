#include "core/ValueBinding.h"

namespace PixelUIValue {
namespace {

bool appendUnsigned(
    char* buffer,
    size_t bufferSize,
    size_t& output,
    uint32_t value,
    uint8_t minimumDigits) {
    char reversed[10];
    size_t digitCount = 0U;
    do {
        reversed[digitCount++] = static_cast<char>('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U);

    while (digitCount < minimumDigits) reversed[digitCount++] = '0';
    if (output + digitCount >= bufferSize) return false;
    while (digitCount != 0U) buffer[output++] = reversed[--digitCount];
    return true;
}

uint32_t powerOfTen(uint8_t exponent) {
    uint32_t value = 1U;
    while (exponent-- != 0U) value *= 10U;
    return value;
}

} // namespace

bool formatScaledInteger(
    char* buffer,
    size_t bufferSize,
    int32_t raw,
    uint32_t scale,
    uint8_t fractionalDigits,
    const char* suffix) {
    const auto fail = [buffer, bufferSize]() {
        if (buffer != nullptr && bufferSize != 0U) buffer[0] = '\0';
        return false;
    };
    if (buffer == nullptr || bufferSize == 0U || scale == 0U ||
        fractionalDigits > 9U) {
        return fail();
    }
    buffer[0] = '\0';

    const bool negative = raw < 0;
    const uint32_t magnitude = negative
        ? 0U - static_cast<uint32_t>(raw)
        : static_cast<uint32_t>(raw);
    uint32_t whole = magnitude / scale;
    const uint32_t remainder = magnitude % scale;

    const uint32_t fractionalScale = powerOfTen(fractionalDigits);
    uint32_t fractional = static_cast<uint32_t>(
        (static_cast<uint64_t>(remainder) * fractionalScale + (scale / 2U)) /
        scale);
    if (fractional == fractionalScale) {
        ++whole;
        fractional = 0U;
    }

    size_t output = 0U;
    if (negative) {
        if (output + 1U >= bufferSize) return fail();
        buffer[output++] = '-';
    }
    if (!appendUnsigned(buffer, bufferSize, output, whole, 1U)) return fail();

    if (fractionalDigits != 0U) {
        if (output + 1U >= bufferSize) return fail();
        buffer[output++] = '.';
        if (!appendUnsigned(
                buffer,
                bufferSize,
                output,
                fractional,
                fractionalDigits)) return fail();
    }

    if (suffix != nullptr) {
        for (size_t i = 0U; suffix[i] != '\0'; ++i) {
            if (output + 1U >= bufferSize) return fail();
            buffer[output++] = suffix[i];
        }
    }

    buffer[output] = '\0';
    return true;
}

} // namespace PixelUIValue
