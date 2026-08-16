/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupBase.h"
#include "config.h"
#include "core/Callbacks.h"
#include "core/NumericFormatter.h"
#include "core/NumericRange.h"
#include "core/ValueEdit.h"
#include "focus/focus.h"
#include "widgets/num_scroll/num_scroll.h"
#include <etl/array.h>
#include <etl/pool.h>

class PopupValueDigits : public PopupBase {
private:
    static_assert(MAX_INT_FIXED_WIDTH > 0, "PopupValueDigits needs at least one digit");
    static_assert(MAX_INT_FIXED_WIDTH <= 9, "PopupValueDigits must fit in int32_t");
    static_assert(MAX_ONSCREEN_WIDGET_NUM >= MAX_INT_FIXED_WIDTH,
                  "FocusManager capacity must cover every digit");

    static constexpr uint8_t DIGIT_WIDTH = 12;
    static constexpr uint8_t DIGIT_HEIGHT = 16;
    static constexpr uint8_t DIGIT_GAP = 2;

    // title_, binding contexts in ownedSession_, and an injected session are
    // non-owning and must outlive this Popup.
    const char* title_;
    ValueCallback compatibilityCallback_;
    ValueEditSession ownedSession_;
    ValueEditSession* session_ = nullptr;
    NumericRange digitRange_{};
    static constexpr IntegerFormat DIGIT_FORMAT{1U, nullptr};
    etl::pool<NumScroll, MAX_INT_FIXED_WIDTH> digitPool_;
    etl::array<NumScroll*, MAX_INT_FIXED_WIDTH> digits_{};
    uint8_t digitCount_ = 0;
    FocusManager focusManager_;

    static int32_t maximumValue(uint8_t digitCount);
    int32_t collectValue() const;
    bool synchronizeValue();
    bool commitEditing();
    bool cancelEditing();
    void initializeDigits();

protected:
    void drawContent(const PopupContentBounds& bounds) override;
    bool handleContentInput(InputEvent event) override;

public:
    PopupValueDigits(PixelUI& ui, uint16_t width, uint16_t height,
                     ValueEditSession& session, uint8_t digitCount,
                     const char* title = "", uint16_t duration = 3000);
    PopupValueDigits(PixelUI& ui, uint16_t width, uint16_t height,
                     ValueEditorBinding binding, uint8_t digitCount,
                     const char* title, uint16_t duration,
                     ValueCallback callback = nullptr,
                     ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm);
    PopupValueDigits(PixelUI& ui, uint16_t width, uint16_t height,
                     int32_t& value, uint8_t digitCount,
                     const char* title = "", uint16_t duration = 3000,
                     ValueCallback callback = nullptr);
    ~PopupValueDigits();

    static bool isValidDigitCount(uint8_t digitCount) {
        return digitCount > 0U && digitCount <= MAX_INT_FIXED_WIDTH;
    }
};
