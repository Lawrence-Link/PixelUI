#include "PixelUI.h"
#include "core/FixedPoint.h"
#include "widgets/curve_chart/curve_chart.h"
#include "widgets/histogram/histogram.h"

#include <stdint.h>
#include <string.h>

namespace {

struct GuardedBuffer {
    uint32_t before = 0x12345678U;
    ChartValue data[8]{};
    uint32_t after = 0x87654321U;
};

template <typename Chart>
bool verifyChartBuffer(Chart& chart, GuardedBuffer& storage) {
    for (size_t i = 0; i < 8U; ++i) {
        storage.data[i] = chartValueFromInt(42);
    }

    chart.clearData();
    for (size_t i = 0; i < 8U; ++i) {
        if (storage.data[i] != 0) return false;
    }

    for (int value = 1; value <= 9; ++value) {
        chart.addData(chartValueFromInt(value));
    }

    return (storage.before == 0x12345678U) &&
           (storage.after == 0x87654321U) &&
           (chart.getMinValueInWindow() == chartValueFromInt(2)) &&
           (chart.getMaxValueInWindow() == chartValueFromInt(9)) &&
           (chart.getAverageValueInWindow() == chartValueFromMilli(5500)) &&
           (chart.getMinValueInHistory() == chartValueFromInt(1)) &&
           (chart.getMaxValueInHistory() == chartValueFromInt(9)) &&
           (chart.getAverageValueInHistory() == chartValueFromInt(5));
}

bool verifyFixedPointFormatting() {
    char buffer[16]{};
    if (!PixelUIFixedPoint::formatDecimal1(buffer, sizeof(buffer), 123) ||
        strcmp(buffer, "12.3") != 0) return false;
    if (!PixelUIFixedPoint::formatDecimal1(buffer, sizeof(buffer), -7) ||
        strcmp(buffer, "-0.7") != 0) return false;
    if (!PixelUIFixedPoint::formatDecimal1(buffer, sizeof(buffer), INT32_MIN) ||
        strcmp(buffer, "-214748364.8") != 0) return false;

    char tooSmall[4] = {'x', 'x', 'x', '\0'};
    return !PixelUIFixedPoint::formatDecimal1(tooSmall, sizeof(tooSmall), 123) &&
           tooSmall[0] == '\0';
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

    histogram.clearData();
    histogram.addData(chartValueFromInt(-4));
    histogram.addData(chartValueFromInt(-2));
    if (histogram.getMaxValueInWindow() != chartValueFromInt(-2) ||
        histogram.getMinValueInWindow() != chartValueFromInt(-4)) return 3;

    if (!verifyFixedPointFormatting()) return 4;

    return 0;
}
