#include "PixelUI.h"
#include "widgets/ChartSeries.h"
#include "widgets/curve_chart/curve_chart.h"
#include "widgets/histogram/histogram.h"

#include <stdint.h>

namespace {

struct GuardedSeries {
    uint32_t before = 0x12345678U;
    StaticChartSeries<8> series;
    uint32_t after = 0x87654321U;
};

bool verifyWindowAndHistory(GuardedSeries& guarded) {
    ChartSeries& series = guarded.series;
    for (ChartSample sample = 1; sample <= 9; ++sample) series.add(sample);

    if (guarded.before != 0x12345678U || guarded.after != 0x87654321U ||
        series.capacity() != 8U || series.size() != 8U ||
        series.minimum() != 2 || series.maximum() != 9 ||
        series.average() != 5 || series.historyMinimum() != 1 ||
        series.historyMaximum() != 9 || series.historyAverage() != 5 ||
        series.historyCount() != 9U) {
        return false;
    }

    for (size_t offset = 0U; offset < series.size(); ++offset) {
        if (series.sampleFromNewest(offset) !=
            static_cast<ChartSample>(9U - offset)) {
            return false;
        }
    }
    return series.sampleFromNewest(series.size()) == 0;
}

bool verifyNegativeValuesAndClear(StaticChartSeries<8>& series) {
    series.clear();
    series.add(-4);
    series.add(-2);
    if (series.minimum() != -4 || series.maximum() != -2 ||
        series.average() != -3 || series.historyMinimum() != -4 ||
        series.historyMaximum() != -2 || series.historyAverage() != -3) {
        return false;
    }

    const uint32_t revision = series.revision();
    series.clear();
    return series.empty() && series.size() == 0U &&
           series.minimum() == 0 && series.maximum() == 0 &&
           series.average() == 0 && series.historyCount() == 0U &&
           series.historyMinimum() == 0 && series.historyMaximum() == 0 &&
           series.historyAverage() == 0 && series.revision() == revision + 1U;
}

} // namespace

int main() {
    GuardedSeries guarded;
    if (!verifyWindowAndHistory(guarded)) return 1;
    if (!verifyNegativeValuesAndClear(guarded.series)) return 2;

    U8G2 display;
    PixelUI ui(display);
    Histogram histogram(
        ui,
        0,
        0,
        4,
        8,
        guarded.series,
        ChartExpandSize<8, 16>{},
        EXPAND_BASE::TOP_LEFT);
    CurveChart curve(
        ui,
        0,
        0,
        4,
        8,
        guarded.series,
        ChartExpandSize<8, 16>{},
        EXPAND_BASE::TOP_LEFT);
    (void)histogram;
    (void)curve;

    return 0;
}
