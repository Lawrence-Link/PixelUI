#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/NumericRange.h"

class FixedBufferWriter {
public:
    // buffer is non-owning and must remain valid for the writer's lifetime.
    FixedBufferWriter(char* buffer, size_t capacity);

    bool append(const char* text);
    bool appendCharacter(char character);
    bool appendInteger(int32_t value, uint8_t minimumDigits = 1U);
    bool appendUnsigned(uint32_t value, uint8_t minimumDigits = 1U);
    bool finish();

    bool valid() const { return valid_; }
    size_t size() const { return position_; }

private:
    bool reserve(size_t count);
    bool fail();

    char* buffer_ = nullptr;
    size_t capacity_ = 0U;
    size_t position_ = 0U;
    bool valid_ = false;
};

struct IntegerFormat {
    uint8_t minimumDigits = 1U;
    const char* suffix = nullptr;
};

struct ScaledIntegerFormat {
    uint32_t scale = 1U;
    uint8_t fractionalDigits = 0U;
    uint8_t minimumIntegerDigits = 1U;
    const char* suffix = nullptr;
};

struct PercentageFormat {
    const NumericRange* range = nullptr;
    const char* suffix = "%";
};

class NumericFormatter {
public:
    using FormatFunction = bool (*)(
        const void* context,
        int32_t value,
        char* buffer,
        size_t bufferSize);

    constexpr NumericFormatter() = default;

    // context is non-owning and must outlive every format() call made through
    // the returned formatter. The same rule applies to suffix/range pointers
    // stored inside IntegerFormat, ScaledIntegerFormat and PercentageFormat.
    static constexpr NumericFormatter custom(
        const void* context,
        FormatFunction function) {
        return NumericFormatter(context, function);
    }

    static constexpr NumericFormatter integer() {
        return NumericFormatter(nullptr, &formatInteger);
    }

    static constexpr NumericFormatter integer(const IntegerFormat& format) {
        return NumericFormatter(&format, &formatInteger);
    }

    static constexpr NumericFormatter scaled(const ScaledIntegerFormat& format) {
        return NumericFormatter(&format, &formatScaled);
    }

    static constexpr NumericFormatter percentage(const PercentageFormat& format) {
        return NumericFormatter(&format, &formatPercentage);
    }

    bool format(int32_t value, char* buffer, size_t bufferSize) const;
    constexpr bool valid() const { return function_ != nullptr; }
    constexpr const void* context() const { return context_; }
    constexpr FormatFunction function() const { return function_; }

private:
    constexpr NumericFormatter(const void* context, FormatFunction function)
        : context_(context), function_(function) {}

    static bool formatInteger(
        const void* context, int32_t value, char* buffer, size_t bufferSize);
    static bool formatScaled(
        const void* context, int32_t value, char* buffer, size_t bufferSize);
    static bool formatPercentage(
        const void* context, int32_t value, char* buffer, size_t bufferSize);

    const void* context_ = nullptr;
    FormatFunction function_ = nullptr;
};
