/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the conditions in LICENSE are met.
 */

#pragma once

#include "U8g2lib.h"
#include "core/CommonTypes.h"
#include <stdint.h>

/**
 * @brief Viewport state for a canvas that can be taller than the display.
 *
 * Coordinates stored by applications are canvas coordinates. The camera
 * offset is subtracted only by Canvas draw calls; direct U8G2 drawing remains
 * in fixed screen coordinates.
 */
class CanvasCamera {
public:
    CanvasCamera(int32_t viewportWidth, int32_t viewportHeight)
        : viewportWidth_(viewportWidth), viewportHeight_(viewportHeight) {}

    void setEnabled(bool enabled);
    bool isEnabled() const { return enabled_; }

    int32_t x() const { return enabled_ ? x_ : 0; }
    int32_t y() const { return enabled_ ? y_ : 0; }
    int32_t storedY() const { return y_; }

    bool setY(int32_t y);
    bool scrollBy(int32_t deltaY);
    bool ensureVisible(int32_t top, int32_t bottom);
    bool handleInput(InputEvent event);

    void setContentHeight(int32_t height);
    int32_t contentHeight() const { return contentHeight_; }
    int32_t maxY() const;
    int32_t viewportWidth() const { return viewportWidth_; }
    int32_t viewportHeight() const { return viewportHeight_; }

private:
    int32_t clampY(int32_t y) const;

    int32_t viewportWidth_ = 0;
    int32_t viewportHeight_ = 0;
    int32_t x_ = 0;
    int32_t y_ = 0;
    int32_t contentHeight_ = 0;
    bool enabled_ = false;
};

/**
 * @brief Camera-aware drawing facade for the coordinate-bearing U8G2 APIs.
 *
 * Draw on this object when content should move with the application camera.
 * Use PixelUI::getU8G2() for fixed overlays and low-level display operations.
 */
class Canvas {
public:
    Canvas(U8G2& display, int32_t displayWidth, int32_t displayHeight)
        : display_(display), camera_(displayWidth, displayHeight),
          displayWidth_(displayWidth), displayHeight_(displayHeight) {}

    CanvasCamera& camera() { return camera_; }
    const CanvasCamera& camera() const { return camera_; }
    U8G2& rawDisplay() { return display_; }

    void beginFrame();
    bool endFrame();
    void setContentHeight(int32_t height);

    int32_t getDisplayWidth() const { return displayWidth_; }
    int32_t getDisplayHeight() const { return displayHeight_; }
    int32_t getWidth() const { return displayWidth_; }

    void setDrawColor(uint8_t color) { display_.setDrawColor(color); }
    uint8_t getDrawColor() { return display_.getDrawColor(); }
    void setBitmapMode(uint8_t transparent) { display_.setBitmapMode(transparent); }
    void setFont(const uint8_t* font) { display_.setFont(font); }
    void setFontMode(uint8_t transparent) { display_.setFontMode(transparent); }
    void setFontDirection(uint8_t direction) { display_.setFontDirection(direction); }
    void setFontPosBaseline() { display_.setFontPosBaseline(); }
    void setFontPosBottom() { display_.setFontPosBottom(); }
    void setFontPosTop() { display_.setFontPosTop(); }
    void setFontPosCenter() { display_.setFontPosCenter(); }
    void setFontRefHeightText() { display_.setFontRefHeightText(); }
    void setFontRefHeightExtendedText() { display_.setFontRefHeightExtendedText(); }
    void setFontRefHeightAll() { display_.setFontRefHeightAll(); }
    int8_t getAscent() { return display_.getAscent(); }
    int8_t getDescent() { return display_.getDescent(); }
    int8_t getFontAscent() { return display_.getFontAscent(); }
    int8_t getFontDescent() { return display_.getFontDescent(); }
    u8g2_uint_t getStrWidth(const char* text) { return display_.getStrWidth(text); }
    u8g2_uint_t getUTF8Width(const char* text) { return display_.getUTF8Width(text); }
    int8_t getXOffsetGlyph(uint16_t encoding) { return display_.getXOffsetGlyph(encoding); }
    int8_t getXOffsetUTF8(const char* text) { return display_.getXOffsetUTF8(text); }

    void drawPixel(int32_t x, int32_t y);
    void drawHLine(int32_t x, int32_t y, int32_t width);
    void drawVLine(int32_t x, int32_t y, int32_t height);
    void drawHVLine(int32_t x, int32_t y, int32_t length, uint8_t direction);
    void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void drawFrame(int32_t x, int32_t y, int32_t width, int32_t height);
    void drawRFrame(int32_t x, int32_t y, int32_t width, int32_t height, int32_t radius);
    void drawBox(int32_t x, int32_t y, int32_t width, int32_t height);
    void drawRBox(int32_t x, int32_t y, int32_t width, int32_t height, int32_t radius);
    void drawCircle(int32_t x, int32_t y, int32_t radius, uint8_t option = U8G2_DRAW_ALL);
    void drawDisc(int32_t x, int32_t y, int32_t radius, uint8_t option = U8G2_DRAW_ALL);
    void drawEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, uint8_t option = U8G2_DRAW_ALL);
    void drawFilledEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, uint8_t option = U8G2_DRAW_ALL);
    void drawArc(int32_t x, int32_t y, int32_t radius, uint8_t start, uint8_t end);
    void drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void drawBitmap(int32_t x, int32_t y, int32_t byteWidth, int32_t height, const uint8_t* bitmap);
    void drawXBM(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t* bitmap);
    void drawXBMP(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t* bitmap);
    u8g2_uint_t drawGlyph(int32_t x, int32_t y, uint16_t encoding);
    u8g2_uint_t drawGlyphX2(int32_t x, int32_t y, uint16_t encoding);
    u8g2_uint_t drawStr(int32_t x, int32_t y, const char* text);
    u8g2_uint_t drawStrX2(int32_t x, int32_t y, const char* text);
    u8g2_uint_t drawUTF8(int32_t x, int32_t y, const char* text);
    u8g2_uint_t drawUTF8X2(int32_t x, int32_t y, const char* text);
    u8g2_uint_t drawExtUTF8(int32_t x, int32_t y, uint8_t toLeft,
                           const uint16_t* kerningTable, const char* text);
    void drawButtonUTF8(int32_t x, int32_t y, int32_t flags, int32_t width,
                        int32_t horizontalPadding, int32_t verticalPadding,
                        const char* text);

#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
    void setClipWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void setMaxClipWindow() { display_.setMaxClipWindow(); }
#endif

private:
    u8g2_uint_t screenX(int32_t x) const;
    u8g2_uint_t screenY(int32_t y) const;
    void observeBottom(int32_t bottom);
    void observeRect(int32_t y, int32_t height);
    void observeText(int32_t baseline, bool doubled = false);

    U8G2& display_;
    CanvasCamera camera_;
    int32_t displayWidth_ = 0;
    int32_t displayHeight_ = 0;
    int32_t observedHeight_ = 0;
    int32_t declaredHeight_ = 0;
};
