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
                                   ValueEditSession& session, uint8_t digitCount,
                                   const char* title, uint16_t duration)
    : PopupBase(ui, width, height, duration),
      title_(title),
      ownedSession_(0),
      session_(&session),
      digitCount_(isValidDigitCount(digitCount) ? digitCount : 1U),
      focusManager_(ui) {
    initializeDigits();
}

PopupValueDigits::PopupValueDigits(PixelUI& ui, uint16_t width, uint16_t height,
                                   ValueEditorBinding binding, uint8_t digitCount,
                                   const char* title, uint16_t duration,
                                   ValueCallback callback, ValueEditPolicy policy)
    : PopupBase(ui, width, height, duration),
      title_(title),
      compatibilityCallback_(etl::move(callback)),
      ownedSession_(binding, policy),
      session_(&ownedSession_),
      digitCount_(isValidDigitCount(digitCount) ? digitCount : 1U),
      focusManager_(ui) {
    initializeDigits();
}

PopupValueDigits::PopupValueDigits(PixelUI& ui, uint16_t width, uint16_t height,
                                   int32_t& value, uint8_t digitCount,
                                   const char* title, uint16_t duration,
                                   ValueCallback callback)
    : PopupBase(ui, width, height, duration),
      title_(title),
      compatibilityCallback_(etl::move(callback)),
      ownedSession_(
          ValueEditorBinding::reference(value),
          ValueEditPolicy::CommitOnConfirm),
      session_(&ownedSession_),
      digitCount_(isValidDigitCount(digitCount) ? digitCount : 1U),
      focusManager_(ui) {
    initializeDigits();
}

void PopupValueDigits::initializeDigits() {
    NumericRange::tryCreate(0, 9, 1, digitRange_);
    const int32_t maximum = maximumValue(digitCount_);
    const int32_t sourceValue = session_ != nullptr && session_->valid()
        ? session_->draftValue()
        : 0;
    const int32_t displayedValue = sourceValue < 0
        ? 0
        : sourceValue > maximum ? maximum : sourceValue;

    const int32_t totalWidth = digitCount_ * DIGIT_WIDTH + (digitCount_ - 1U) * DIGIT_GAP;
    const int32_t startX = (ui().getDisplayWidth() - totalWidth) / 2;
    const int32_t digitY = title_ && title_[0] != '\0'
        ? ui().getDisplayHeight() / 2
        : (ui().getDisplayHeight() - DIGIT_HEIGHT) / 2;

    int32_t divisor = 1;
    for (uint8_t i = 1; i < digitCount_; ++i) divisor *= 10;
    for (uint8_t index = 0; index < digitCount_; ++index) {
        NumScroll* digit = digitPool_.create(
            ui(),
            static_cast<uint16_t>(startX + index * (DIGIT_WIDTH + DIGIT_GAP)),
            static_cast<uint16_t>(digitY),
            static_cast<uint16_t>(DIGIT_WIDTH),
            static_cast<uint16_t>(DIGIT_HEIGHT),
            digitRange_,
            NumericFormatter::integer(DIGIT_FORMAT));
        if (digit == nullptr) continue;
        digits_[index] = digit;
        digit->setPresentation(NumScroll::Presentation::Bare);
        digit->setValueImmediate((displayedValue / divisor) % 10);
        digit->onLoadNoAnim();
        focusManager_.addWidget(digit);
        divisor /= 10;
    }
}

PopupValueDigits::~PopupValueDigits() {
    focusManager_.clear();
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

bool PopupValueDigits::synchronizeValue() {
    if (session_ == nullptr || !session_->valid()) return false;
    const int32_t newValue = collectValue();
    if (newValue == session_->draftValue()) return true;
    if (!session_->setDraftValue(newValue)) return false;
    if (compatibilityCallback_ && session_->policy() == ValueEditPolicy::Live) {
        compatibilityCallback_(newValue);
    }
    ui().markDirty();
    return true;
}

bool PopupValueDigits::commitEditing() {
    if (!synchronizeValue()) return false;
    const bool changed = session_->draftValue() != session_->originalValue();
    if (!session_->commit()) return false;
    if (changed && compatibilityCallback_ &&
        session_->policy() == ValueEditPolicy::CommitOnConfirm) {
        compatibilityCallback_(session_->draftValue());
    }
    return true;
}

bool PopupValueDigits::cancelEditing() {
    if (session_ == nullptr) return false;
    const bool changed = session_->draftValue() != session_->originalValue();
    const int32_t original = session_->originalValue();
    if (!session_->cancel()) return false;
    if (changed && compatibilityCallback_ &&
        session_->policy() == ValueEditPolicy::Live) {
        compatibilityCallback_(original);
    }
    return true;
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
            if (cancelEditing()) requestClose();
            return true;
        }
        const bool finished = activeWidget->handleEvent(event);
        if (!synchronizeValue()) return true;
        if (finished) {
            focusManager_.clearActiveWidget();
            if (commitEditing()) requestClose();
        }
        return true;
    }

    switch (event) {
        case InputEvent::BACK:
            if (cancelEditing()) requestClose();
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
