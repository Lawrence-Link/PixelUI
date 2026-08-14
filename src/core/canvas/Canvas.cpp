#include "core/canvas/Canvas.h"

namespace {
int32_t maximum(int32_t a, int32_t b) { return (a > b) ? a : b; }
int32_t minimum(int32_t a, int32_t b) { return (a < b) ? a : b; }
}

void CanvasCamera::setEnabled(bool enabled) {
    enabled_ = enabled;
    y_ = clampY(y_);
}

int32_t CanvasCamera::maxY() const {
    return maximum(0, contentHeight_ - viewportHeight());
}

int32_t CanvasCamera::clampY(int32_t y) const {
    return maximum(0, minimum(y, maxY()));
}

bool CanvasCamera::setY(int32_t y) {
    const int32_t next = clampY(y);
    if (next == y_) return false;
    y_ = next;
    return true;
}

bool CanvasCamera::scrollBy(int32_t deltaY) {
    return enabled_ && setY(y_ + deltaY);
}

bool CanvasCamera::ensureVisible(int32_t top, int32_t bottom) {
    if (!enabled_) return false;
    if (bottom < top) {
        const int32_t swap = top;
        top = bottom;
        bottom = swap;
    }

    int32_t target = y_;
    if (top < y_) {
        target = top;
    } else if (bottom > y_ + viewportHeight()) {
        target = bottom - viewportHeight();
    }
    return setY(target);
}

bool CanvasCamera::handleInput(InputEvent event) {
    if (!enabled_) return false;
    if (event == InputEvent::UP) return scrollBy(-viewportHeight());
    if (event == InputEvent::DOWN) return scrollBy(viewportHeight());
    return false;
}

void CanvasCamera::setContentHeight(int32_t height) {
    contentHeight_ = maximum(viewportHeight(), height);
    y_ = clampY(y_);
}

void Canvas::beginFrame() {
    observedHeight_ = getDisplayHeight();
    declaredHeight_ = 0;
}

bool Canvas::endFrame() {
    const int32_t previousY = camera_.storedY();
    camera_.setContentHeight(maximum(observedHeight_, declaredHeight_));
    return previousY != camera_.storedY();
}

void Canvas::setContentHeight(int32_t height) {
    declaredHeight_ = maximum(declaredHeight_, height);
}

u8g2_uint_t Canvas::screenX(int32_t x) const {
    return static_cast<u8g2_uint_t>(x - camera_.x());
}

u8g2_uint_t Canvas::screenY(int32_t y) const {
    return static_cast<u8g2_uint_t>(y - camera_.y());
}

void Canvas::observeBottom(int32_t bottom) {
    observedHeight_ = maximum(observedHeight_, bottom);
}

void Canvas::observeRect(int32_t y, int32_t height) {
    if (height > 0) observeBottom(y + height);
}

void Canvas::observeText(int32_t baseline, bool doubled) {
    const int32_t scale = doubled ? 2 : 1;
    observeBottom(baseline - static_cast<int32_t>(display_.getDescent()) * scale + 1);
}

void Canvas::drawPixel(int32_t x, int32_t y) {
    observeBottom(y + 1);
    display_.drawPixel(screenX(x), screenY(y));
}

void Canvas::drawHLine(int32_t x, int32_t y, int32_t width) {
    observeBottom(y + 1);
    display_.drawHLine(screenX(x), screenY(y), static_cast<u8g2_uint_t>(width));
}

void Canvas::drawVLine(int32_t x, int32_t y, int32_t height) {
    observeRect(y, height);
    display_.drawVLine(screenX(x), screenY(y), static_cast<u8g2_uint_t>(height));
}

void Canvas::drawHVLine(int32_t x, int32_t y, int32_t length, uint8_t direction) {
    observeBottom(y + ((direction == 0U || direction == 2U) ? 1 : length));
    display_.drawHVLine(screenX(x), screenY(y), static_cast<u8g2_uint_t>(length), direction);
}

void Canvas::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    observeBottom(maximum(y1, y2) + 1);
    display_.drawLine(screenX(x1), screenY(y1), screenX(x2), screenY(y2));
}

void Canvas::drawFrame(int32_t x, int32_t y, int32_t width, int32_t height) {
    observeRect(y, height);
    display_.drawFrame(screenX(x), screenY(y), width, height);
}

void Canvas::drawRFrame(int32_t x, int32_t y, int32_t width, int32_t height, int32_t radius) {
    observeRect(y, height);
    display_.drawRFrame(screenX(x), screenY(y), width, height, radius);
}

void Canvas::drawBox(int32_t x, int32_t y, int32_t width, int32_t height) {
    observeRect(y, height);
    display_.drawBox(screenX(x), screenY(y), width, height);
}

void Canvas::drawRBox(int32_t x, int32_t y, int32_t width, int32_t height, int32_t radius) {
    observeRect(y, height);
    display_.drawRBox(screenX(x), screenY(y), width, height, radius);
}

void Canvas::drawCircle(int32_t x, int32_t y, int32_t radius, uint8_t option) {
    observeBottom(y + radius + 1);
    display_.drawCircle(screenX(x), screenY(y), radius, option);
}

void Canvas::drawDisc(int32_t x, int32_t y, int32_t radius, uint8_t option) {
    observeBottom(y + radius + 1);
    display_.drawDisc(screenX(x), screenY(y), radius, option);
}

void Canvas::drawEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, uint8_t option) {
    observeBottom(y + ry + 1);
    display_.drawEllipse(screenX(x), screenY(y), rx, ry, option);
}

void Canvas::drawFilledEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, uint8_t option) {
    observeBottom(y + ry + 1);
    display_.drawFilledEllipse(screenX(x), screenY(y), rx, ry, option);
}

void Canvas::drawArc(int32_t x, int32_t y, int32_t radius, uint8_t start, uint8_t end) {
    observeBottom(y + radius + 1);
    display_.drawArc(screenX(x), screenY(y), radius, start, end);
}

void Canvas::drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    observeBottom(maximum(y0, maximum(y1, y2)) + 1);
    display_.drawTriangle(screenX(x0), screenY(y0), screenX(x1), screenY(y1), screenX(x2), screenY(y2));
}

void Canvas::drawBitmap(int32_t x, int32_t y, int32_t byteWidth, int32_t height, const uint8_t* bitmap) {
    observeRect(y, height);
    display_.drawBitmap(screenX(x), screenY(y), byteWidth, height, bitmap);
}

void Canvas::drawXBM(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t* bitmap) {
    observeRect(y, height);
    display_.drawXBM(screenX(x), screenY(y), width, height, bitmap);
}

void Canvas::drawXBMP(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t* bitmap) {
    observeRect(y, height);
    display_.drawXBMP(screenX(x), screenY(y), width, height, bitmap);
}

u8g2_uint_t Canvas::drawGlyph(int32_t x, int32_t y, uint16_t encoding) {
    observeText(y);
    return display_.drawGlyph(screenX(x), screenY(y), encoding);
}

u8g2_uint_t Canvas::drawGlyphX2(int32_t x, int32_t y, uint16_t encoding) {
    observeText(y, true);
    return display_.drawGlyphX2(screenX(x), screenY(y), encoding);
}

u8g2_uint_t Canvas::drawStr(int32_t x, int32_t y, const char* text) {
    observeText(y);
    return display_.drawStr(screenX(x), screenY(y), text);
}

u8g2_uint_t Canvas::drawStrX2(int32_t x, int32_t y, const char* text) {
    observeText(y, true);
    return display_.drawStrX2(screenX(x), screenY(y), text);
}

u8g2_uint_t Canvas::drawUTF8(int32_t x, int32_t y, const char* text) {
    observeText(y);
    return display_.drawUTF8(screenX(x), screenY(y), text);
}

u8g2_uint_t Canvas::drawUTF8X2(int32_t x, int32_t y, const char* text) {
    observeText(y, true);
    return display_.drawUTF8X2(screenX(x), screenY(y), text);
}

u8g2_uint_t Canvas::drawExtUTF8(int32_t x, int32_t y, uint8_t toLeft,
                               const uint16_t* kerningTable, const char* text) {
    observeText(y);
    return display_.drawExtUTF8(screenX(x), screenY(y), toLeft, kerningTable, text);
}

void Canvas::drawButtonUTF8(int32_t x, int32_t y, int32_t flags, int32_t width,
                            int32_t horizontalPadding, int32_t verticalPadding,
                            const char* text) {
    observeBottom(y + verticalPadding + 1);
    display_.drawButtonUTF8(screenX(x), screenY(y), flags, width,
                            horizontalPadding, verticalPadding, text);
}

#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
void Canvas::setClipWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    display_.setClipWindow(screenX(x0), screenY(y0), screenX(x1), screenY(y1));
}
#endif
