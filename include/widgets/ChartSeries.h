#pragma once

#include <stddef.h>
#include <stdint.h>

using ChartSample = int32_t;

template <size_t Capacity>
class StaticChartSeries;

class ChartSeries {
public:
    ChartSeries(const ChartSeries&) = delete;
    ChartSeries& operator=(const ChartSeries&) = delete;
    ChartSeries(ChartSeries&&) = delete;
    ChartSeries& operator=(ChartSeries&&) = delete;

    void add(ChartSample sample);
    void clear();

    size_t capacity() const { return capacity_; }
    size_t size() const { return count_; }
    bool empty() const { return count_ == 0U; }
    uint32_t revision() const { return revision_; }

    ChartSample sampleFromNewest(size_t offset) const;

    ChartSample minimum() const;
    ChartSample maximum() const;
    ChartSample average() const;

    ChartSample historyMinimum() const;
    ChartSample historyMaximum() const;
    ChartSample historyAverage() const;
    uint32_t historyCount() const { return historyCount_; }

private:
    template <size_t Capacity>
    friend class StaticChartSeries;

    ChartSeries(ChartSample* storage, size_t capacity)
        : storage_(storage), capacity_(capacity) {}

    void recalculateExtremes();

    ChartSample* storage_ = nullptr;
    size_t capacity_ = 0U;
    size_t writeIndex_ = 0U;
    size_t count_ = 0U;
    int64_t sum_ = 0;
    ChartSample minimum_ = 0;
    ChartSample maximum_ = 0;

    int64_t historySum_ = 0;
    ChartSample historyMinimum_ = 0;
    ChartSample historyMaximum_ = 0;
    uint32_t historyCount_ = 0U;
    uint32_t revision_ = 0U;
};

template <size_t Capacity>
class StaticChartSeries : public ChartSeries {
    static_assert(Capacity > 0U, "Chart series capacity must be greater than zero");

public:
    static constexpr size_t staticCapacity = Capacity;

    StaticChartSeries() : ChartSeries(storage_, Capacity) {}

private:
    ChartSample storage_[Capacity];
};

template <uint16_t Width, uint16_t Height>
struct ChartExpandSize {
    static_assert(Width > 0U, "Expanded chart width must be greater than zero");
    static_assert(Height > 0U, "Expanded chart height must be greater than zero");

    static constexpr uint16_t width = Width;
    static constexpr uint16_t height = Height;
};
