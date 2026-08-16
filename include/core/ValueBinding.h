#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/ScaledInteger.h"

namespace PixelUIValue {

bool formatScaledInteger(
    char* buffer,
    size_t bufferSize,
    int32_t raw,
    uint32_t scale,
    uint8_t fractionalDigits,
    const char* suffix = nullptr);

class Binding {
public:
    using FormatFunction = bool (*)(
        const void* object,
        const char* suffix,
        char* buffer,
        size_t bufferSize);

    constexpr Binding() = default;

    // object and suffix are non-owning and must outlive every format() call.
    static constexpr Binding custom(
        const void* object,
        FormatFunction formatter,
        const char* suffix = nullptr) {
        return Binding(object, formatter, suffix);
    }

    static constexpr Binding integer(
        const int32_t& value,
        const char* suffix = nullptr) {
        return Binding(&value, &formatInteger, suffix);
    }

    template <uint8_t FractionalDigits, int32_t Scale>
    static constexpr Binding decimal(
        const ScaledInt32<Scale>& value,
        const char* suffix = nullptr) {
        static_assert(FractionalDigits <= 9U,
                      "ValueBinding supports at most 9 fractional digits");
        return Binding(
            &value,
            &formatDecimal<FractionalDigits, Scale>,
            suffix);
    }

    bool format(char* buffer, size_t bufferSize) const {
        if (buffer != nullptr && bufferSize != 0U) buffer[0] = '\0';
        if (formatter_ == nullptr) {
            return false;
        }
        const bool formatted =
            formatter_(object_, suffix_, buffer, bufferSize);
        if (!formatted && buffer != nullptr && bufferSize != 0U) {
            buffer[0] = '\0';
        }
        return formatted;
    }

    constexpr const void* object() const { return object_; }
    constexpr FormatFunction formatter() const { return formatter_; }
    constexpr const char* suffix() const { return suffix_; }

private:
    constexpr Binding(
        const void* object,
        FormatFunction formatter,
        const char* suffix)
        : object_(object), formatter_(formatter), suffix_(suffix) {}

    static bool formatInteger(
        const void* object,
        const char* suffix,
        char* buffer,
        size_t bufferSize) {
        if (object == nullptr) return false;
        return formatScaledInteger(
            buffer,
            bufferSize,
            *static_cast<const int32_t*>(object),
            1U,
            0U,
            suffix);
    }

    template <uint8_t FractionalDigits, int32_t Scale>
    static bool formatDecimal(
        const void* object,
        const char* suffix,
        char* buffer,
        size_t bufferSize) {
        if (object == nullptr) return false;
        return formatScaledInteger(
            buffer,
            bufferSize,
            static_cast<const ScaledInt32<Scale>*>(object)->raw(),
            static_cast<uint32_t>(Scale),
            FractionalDigits,
            suffix);
    }

    const void* object_ = nullptr;
    FormatFunction formatter_ = nullptr;
    const char* suffix_ = nullptr;
};

} // namespace PixelUIValue
