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
#include "widgets/text_button/text_button.h"
#include <etl/array.h>
#include <etl/pool.h>

class PopupValueDigits : public PopupBase {
private:
    static_assert(MAX_INT_FIXED_WIDTH > 0, "PopupValueDigits needs at least one digit");
    static_assert(MAX_INT_FIXED_WIDTH <= 9, "PopupValueDigits must fit in int32_t");
    static constexpr uint8_t DIGIT_WIDTH = 12;
    static constexpr uint8_t DIGIT_HEIGHT = 16;
    static constexpr uint8_t DIGIT_GAP = 2;
    static constexpr uint8_t ACTION_HEIGHT = 13;
    static constexpr uint8_t OK_WIDTH = 24;
    static constexpr uint8_t CANCEL_WIDTH = 44;
    static constexpr uint8_t ACTION_GAP = 4;
    static constexpr uint8_t CONTENT_PADDING = 4;

    static constexpr uint16_t minimumHeight() { return 56U; }
    static constexpr uint16_t minimumWidth(uint8_t digitCount) {
        const uint16_t digitWidth = digitCount == 0U
            ? 0U
            : static_cast<uint16_t>(
                  digitCount * DIGIT_WIDTH + (digitCount - 1U) * DIGIT_GAP);
        const uint16_t actionWidth = OK_WIDTH + ACTION_GAP + CANCEL_WIDTH;
        const uint16_t contentWidth = digitWidth > actionWidth
            ? digitWidth
            : actionWidth;
        return static_cast<uint16_t>(contentWidth + 2U * CONTENT_PADDING);
    }

    class ControlGroup final : public IWidget {
    public:
        explicit ControlGroup(PixelUI& ui);
        void setSize(uint16_t width, uint16_t height);
        void onLoad() override {}
        void onOffload() override {}

    private:
        void drawSelf(const WidgetRenderContext&) override {}
        Canvas& display() override;
        PixelUI& ui_;
    };

    enum class FinalizationState : uint8_t {
        Editing,
        Committed,
        Cancelled,
    };

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
    ControlGroup controls_;
    TextButton okButton_;
    TextButton cancelButton_;
    FocusManager focusManager_;
    FinalizationState finalizationState_ = FinalizationState::Editing;

    static int32_t maximumValue(uint8_t digitCount);
    int32_t collectValue() const;
    bool synchronizeValue();
    void restoreDigitsFromDraft();
    bool commitEditing();
    bool cancelEditing(bool closePopup = true);
    void initializeControls();
    void destroyControls();

protected:
    void drawContent(const PopupContentBounds& bounds) override;
    bool handleContentInput(InputEvent event) override;
    void onClosing() override;

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

    static constexpr bool isValidDigitCount(uint8_t digitCount) {
        return digitCount > 0U && digitCount <= MAX_INT_FIXED_WIDTH;
    }
    static constexpr bool isValidLayout(
        uint16_t width, uint16_t height, uint8_t digitCount) {
        return isValidDigitCount(digitCount) &&
               width >= minimumWidth(digitCount) &&
               height >= minimumHeight();
    }
};
