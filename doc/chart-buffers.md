# Chart series and value formatting

## Chart sample ownership

`Histogram` and `CurveChart` are views. They do not allocate storage, define a
unit, or own statistics. The application owns a fixed-capacity
`StaticChartSeries<N>` that must outlive every chart that references it:

```cpp
StaticChartSeries<76> temperatureSeries;
CurveChart chart;

App(PixelUI& ui)
    : chart(
          ui,
          69,
          45,
          56,
          18,
          temperatureSeries,
          ChartExpandSize<76, 63>{},
          EXPAND_BASE::BOTTOM_RIGHT) {}
```

The series is a ring buffer with no heap allocation. Add and clear samples on
the model, not on the widget. `clear()` resets metadata in constant time and
does not sweep the backing array:

```cpp
temperatureSeries.clear();
temperatureSeries.add(215); // The application decides what 215 means.
```

`ChartSample` is an alias of `int32_t`. It is deliberately unitless. A sample
may mean an integer count, tenths of a degree, Q15, or another application
domain representation. The chart only compares and scales samples, so it does
not need to know that representation and does not pull floating-point support
into the rendering path.

Current-window statistics are available through `minimum()`, `maximum()`, and
`average()`. `historyMinimum()`, `historyMaximum()`, and `historyAverage()` cover
all samples since the last `clear()`. Integer averages truncate toward zero.
`sampleFromNewest(0)` returns the latest sample.

The constructor deduces the series capacity and checks at compile time that it
equals the expanded chart width. This preserves one sample slot per expanded
horizontal pixel.

## Domain values and ListView

Use `ScaledInt32<Scale>` when a domain value needs a compile-time scale:

```cpp
using Temperature = ScaledInt32<10>;
Temperature roomTemperature = Temperature::fromRaw(215); // 21.5
```

The scale stays in the domain type. It is not a global chart convention.
`ScaledInt32` stores only one `int32_t`, performs no allocation, and does not
implicitly convert from an unscaled integer.

At the text-rendering boundary, bind the value to a formatter:

```cpp
ListItemAccessory::value(
    PixelUIValue::Binding::decimal<1>(roomTemperature, " C"))
```

`PixelUIValue::Binding` is non-owning. The referenced value and optional suffix
must outlive the `ListItemAccessory`. It contains a data pointer and a plain
formatter function pointer; it does not allocate callback storage. Integer,
scaled-integer, and custom formatters share the same bounded-buffer interface.
The built-in formatting uses integer arithmetic only. A failed formatter leaves
the destination empty, including when a custom formatter reports failure.
