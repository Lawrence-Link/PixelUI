/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include <stdint.h>

namespace PixelUITime {

constexpr uint32_t NO_WAKEUP = UINT32_MAX;
constexpr uint32_t MAX_DEADLINE_INTERVAL = UINT32_MAX / 2U;

inline uint32_t normalizeInterval(uint32_t intervalMs) {
    return (intervalMs == 0U) ? 1U : intervalMs;
}

inline uint32_t normalizeDeadlineInterval(uint32_t intervalMs) {
    intervalMs = normalizeInterval(intervalMs);
    return (intervalMs > MAX_DEADLINE_INTERVAL)
        ? MAX_DEADLINE_INTERVAL
        : intervalMs;
}

/* Absolute deadlines must be less than 2^31 ms into the future. */
inline bool deadlineReached(uint32_t currentTime, uint32_t deadline) {
    return (currentTime - deadline) < (UINT32_MAX / 2U + 1U);
}

inline uint32_t untilDeadline(uint32_t currentTime, uint32_t deadline) {
    return deadlineReached(currentTime, deadline) ? 0U : deadline - currentTime;
}

inline uint32_t earlier(uint32_t lhs, uint32_t rhs) {
    return (lhs < rhs) ? lhs : rhs;
}

} // namespace PixelUITime
