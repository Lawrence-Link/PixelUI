/*
 * Copyright (C) 2025 Lawrence Link
 */

#include "ui/Popup/PopupValueDigits.h"
#include "PixelUI.h"
#include <string.h>

int32_t PopupValueDigits::maximumValue(uint8_t digitCount) {
    int32_t maximum = 0;
    for (uint8_t i = 0; i < digitCount; ++i) maximum = maximum * 10 + 9;
    return maximum;
}

PopupValueDigits::PopupValueDigits(PixelUI& ui, uint16_t width, uint16_t height,
                                   int32_t& value, uint8_t digitCount,
                                   const char* title, uint16_t duration,
                                   ValueCallback callback)
    : PopupBase(ui, width, height, duration),
      value_(value),
      title_(title),
      digitCount_(isValidDigitCount(digitCount) ? digitCount : 1U),
      focusManager_(ui),
      callback_(etl::move(callback)) {
    const int32_t maximum = maximumValue(digitCount_);
    if (value_ < 0) value_ = 0;
    if (value_ > maximum) value_ = maximum;

    const int32_t totalWidth = digitCount_ * DIGIT_WIDTH + (digitCount_ - 1U) * DIGIT_GAP;
    const int32_t startX = (ui.getDisplayWidth() - totalWidth) / 2;
    const int32_t digitY = title_ && title_[0] != '\0'
        ? ui.getDisplayHeight() / 2
        : (ui.getDisplayHeight() - DIGIT_HEIGHT) / 2;

    int32_t divisor = 1;
    for (uint8_t i = 1; i < digitCount_; ++i) divisor *= 10;
    for (uint8_t index = 0; index < digitCount_; ++index) {
        NumScroll* digit = digitPool_.create(
            ui,
            static_cast<uint16_t>(startX + index * (DIGIT_WIDTH + DIGIT_GAP)),
            static_cast<uint16_t>(digitY),
            static_cast<uint16_t>(DIGIT_WIDTH),
            static_cast<uint16_t>(DIGIT_HEIGHT));
        digits_[index] = digit;
        digit->setRange(0, 9);
        digit->setFixedIntDigits(1);
        digit->setPresentation(NumScroll::Presentation::Bare);
        digit->setValueImmediate((value_ / divisor) % 10);
        digit->onLoadNoAnim();
        focusManager_.addWidget(digit);
        divisor /= 10;
    }
}

PopupValueDigits::~PopupValueDigits() {
    focusManager_.clear();
    ui().clearAllAnimations();
    for (uint8_t i = 0; i < digitCount_; ++i) {
        if (digits_[i]) {
            digitPool_.destroy(digits_[i]);
            digits_[i] = nullptr;
        }
    }
    ui().markDirty();
}

int32_t PopupValueDigits::collectValue() const {
    int32_t result = 0;
    for (uint8_t i = 0; i < digitCount_; ++i) {
        result = result * 10 + digits_[i]->getValue();
    }
    return result;
}

void PopupValueDigits::synchronizeValue() {
    const int32_t newValue = collectValue();
    if (newValue == value_) return;
    value_ = newValue;
    if (callback_) callback_(value_);
    ui().markDirty();
}

void PopupValueDigits::drawContent(const PopupContentBounds& bounds) {
    U8G2& u8g2 = ui().getU8G2();
    if (title_ && title_[0] != '\0') {
        u8g2.setFont(PIXELUI_FONT_TEXT);
        const int16_t titleWidth = u8g2.getUTF8Width(title_);
        u8g2.drawUTF8(bounds.centerX - titleWidth / 2, bounds.centerY - 7, title_);
    }

    // Popup geometry is screen-fixed, while NumScroll draws through the
    // camera-aware Canvas. Express the screen bounds in canvas coordinates so
    // Canvas translates the digits back onto the popup without changing the
    // underlying application's camera state.
    const CanvasCamera& camera = ui().getCanvas().camera();
    const WidgetRenderContext context{
        camera.x(), camera.y(),
        {bounds.x + camera.x(), bounds.y + camera.y(),
         bounds.width, bounds.height}};
    for (uint8_t i = 0; i < digitCount_; ++i) digits_[i]->draw(context);
    setContentClip(bounds);
    focusManager_.draw();
}

bool PopupValueDigits::handleContentInput(InputEvent event) {
    IWidget* activeWidget = focusManager_.getActiveWidget();
    if (activeWidget) {
        if (event == InputEvent::BACK) {
            focusManager_.clearActiveWidget();
            ui().markDirty();
            return true;
        }
        if (activeWidget->handleEvent(event)) focusManager_.clearActiveWidget();
        synchronizeValue();
        return true;
    }

    switch (event) {
        case InputEvent::BACK:
            requestClose();
            break;
        case InputEvent::RIGHT:
            focusManager_.moveNext();
            break;
        case InputEvent::LEFT:
            focusManager_.movePrev();
            break;
        case InputEvent::SELECT:
            focusManager_.selectCurrent();
            break;
        default:
            break;
    }
    return true;
}
