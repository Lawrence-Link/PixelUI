#pragma once

#include <stdint.h>

template <int32_t Scale>
class ScaledInt32 {
    static_assert(Scale > 0, "ScaledInt32 scale must be positive");

public:
    static constexpr int32_t scale = Scale;

    constexpr ScaledInt32() = default;

    static constexpr ScaledInt32 fromRaw(int32_t raw) {
        return ScaledInt32(raw);
    }

    constexpr int32_t raw() const { return raw_; }

    friend constexpr bool operator==(ScaledInt32, ScaledInt32) = default;

private:
    explicit constexpr ScaledInt32(int32_t raw) : raw_(raw) {}

    int32_t raw_ = 0;
};
