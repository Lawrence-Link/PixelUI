#include "PixelUI.h"
#include "widgets/curve_chart/curve_chart.h"
#include "widgets/histogram/histogram.h"

#include <stdint.h>

namespace {

struct GuardedBuffer {
    uint32_t before = 0x12345678U;
    float data[8]{};
    uint32_t after = 0x87654321U;
};

template <typename Chart>
bool verifyChartBuffer(Chart& chart, GuardedBuffer& storage) {
    for (size_t i = 0; i < 8U; ++i) {
        storage.data[i] = 42.0f;
    }

    chart.clearData();
    for (size_t i = 0; i < 8U; ++i) {
        if (storage.data[i] != 0.0f) return false;
    }

    for (int value = 1; value <= 9; ++value) {
        chart.addData(static_cast<float>(value));
    }

    return (storage.before == 0x12345678U) &&
           (storage.after == 0x87654321U) &&
           (chart.getMinValueInWindow() == 2.0f) &&
           (chart.getMaxValueInWindow() == 9.0f) &&
           (chart.getAverageValueInWindow() == 5.5f) &&
           (chart.getMinValueInHistory() == 1.0f) &&
           (chart.getMaxValueInHistory() == 9.0f) &&
           (chart.getAverageValueInHistory() == 5.0f);
}

} // namespace

int main() {
    U8G2 display;
    PixelUI ui(display);

    GuardedBuffer histogramStorage;
    Histogram histogram(
        ui,
        0,
        0,
        4,
        8,
        histogramStorage.data,
        ChartExpandSize<8, 16>{},
        EXPAND_BASE::TOP_LEFT);
    if (!verifyChartBuffer(histogram, histogramStorage)) return 1;

    GuardedBuffer curveStorage;
    CurveChart curve(
        ui,
        0,
        0,
        4,
        8,
        curveStorage.data,
        ChartExpandSize<8, 16>{},
        EXPAND_BASE::TOP_LEFT);
    if (!verifyChartBuffer(curve, curveStorage)) return 2;

    return 0;
}

