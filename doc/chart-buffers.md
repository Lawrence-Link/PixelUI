# Histogram and CurveChart buffers

`Histogram` and `CurveChart` do not allocate sample storage. Their owner must
provide a `float` array that outlives the widget. Declare the array before the
widget when both are members of an App:

```cpp
float chartBuffer[76]{};
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

The expanded width and height are template arguments. The constructor deduces
the array capacity and fails at compile time unless the element count exactly
matches the expanded width. The supplied array is the complete circular sample
buffer, with one sample slot per expanded horizontal pixel.
