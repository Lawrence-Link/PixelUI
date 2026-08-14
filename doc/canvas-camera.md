# Canvas camera

PixelUI owns one `Canvas` facade over U8G2. Applications draw scrollable
content in canvas coordinates and the active application's camera translates
those coordinates into the display viewport.

Display width, height, and framebuffer size are read once when `PixelUI` is
constructed. `getDisplayWidth()`, `getDisplayHeight()`, Canvas viewport queries,
and fade processing use those cached values instead of repeatedly querying
U8G2. Configure the U8G2 display before constructing `PixelUI`.

Enable vertical scrolling in the application constructor and draw through
`PixelUI::getCanvas()`:

```cpp
class SettingsApp : public IApplication {
public:
    explicit SettingsApp(PixelUI& ui)
        : IApplication(true), ui_(ui) {}

    void draw() override {
        Canvas& canvas = ui_.getCanvas();
        canvas.setFont(PIXELUI_FONT_TEXT);
        canvas.setContentHeight(180);
        canvas.drawUTF8(4, 12, "General");
        canvas.drawUTF8(4, 92, "Network");
        canvas.drawUTF8(4, 172, "About");
    }

    bool handleInput(InputEvent) override { return false; }

private:
    PixelUI& ui_;
};
```

When `useVerticalScroll` is enabled, an `UP` or `DOWN` event that the
application did not consume moves the camera by one viewport. Applications
with their own selection model can consume those events and call
`ensureCanvasVisible()`, `scrollCanvasTo()`, or `animateCanvasTo()` instead.
`ListView` uses this path to keep its selected row visible.

The wrapped coordinate-bearing APIs include pixels, lines, boxes, rounded
boxes, circles, ellipses, arcs, triangles, bitmaps, glyphs, strings, UTF-8
text, buttons, and clip windows. Font, color, and measurement APIs are also
available on `Canvas`.

`Canvas::setContentHeight()` is optional when every canvas object is drawn on
each frame, because draw calls collect the observed lower bound. Declare it
explicitly for virtualized content or when off-screen objects are skipped.

Direct drawing through `PixelUI::getU8G2()` is intentionally not translated.
Use it for fixed headers, scroll indicators, and other screen overlays. PixelUI
also draws popups and focus overlays in fixed screen coordinates. Buffer setup,
clearing, fade processing, and `sendBuffer()` remain owned by the renderer.

Camera position and content height are saved per application stack depth, so a
paused application returns to its previous scroll position after the top view
is popped.
