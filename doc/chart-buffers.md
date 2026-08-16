# Histogram and CurveChart buffers

`Histogram` and `CurveChart` do not allocate sample storage. Their owner must
provide a `ChartValue` array that outlives the widget. `ChartValue` is a signed
decimal fixed-point value scaled by `CHART_VALUE_SCALE` (1000), so `1250`
represents `1.250`. Declare the array before the widget when both are members of
an App:

```cpp
ChartValue chartBuffer[76]{};
CurveChart chart;

App(PixelUI& ui)
    : chart(
          ui,
          69,
          45,
          56,
          18,
          chartBuffer,
          ChartExpandSize<76, 63>{},
          EXPAND_BASE::BOTTOM_RIGHT) {}
```

Add integer or fractional samples without floating-point arithmetic:

```cpp
chart.addData(chartValueFromInt(42));     // 42.000
chart.addData(chartValueFromMilli(1250)); // 1.250
```

Window and history statistics return `ChartValue` using the same scale.
Average values are truncated toward zero when the exact result cannot be
represented in one-thousandth units.

The expanded width and height are template arguments. The constructor deduces
the array capacity and fails at compile time unless the element count exactly
matches the expanded width. The supplied array is the complete circular sample
buffer, with one sample slot per expanded horizontal pixel.
