/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupBase.h"
#include <stddef.h>
#include "config.h"
#include "core/Callbacks.h"
#include "core/NumericFormatter.h"
#include "core/NumericRange.h"
#include "core/ValueEdit.h"

/**
 * @class PopupProgress
 * @brief A popup displaying a progress bar.
 */
class PopupProgress : public PopupBase {
private:
    NumericRange range_;
    PercentageFormat defaultPercentage_;
    NumericFormatter formatter_;
    // title_, formatter context, binding contexts in ownedSession_, and an
    // injected session are non-owning and must outlive this Popup.
    const char* title_;
    ValueCallback compatibilityCallback_;
    ValueEditSession ownedSession_;
    ValueEditSession* session_ = nullptr;

    bool updateDraft(int32_t value);
    bool commitEditing();
    bool cancelEditing();

public:
    PopupProgress(PixelUI& ui, uint16_t width, uint16_t height,
                  const NumericRange& range, NumericFormatter formatter,
                  ValueEditSession& session, const char* title = "",
                  uint16_t duration = 3000);
    PopupProgress(PixelUI& ui, uint16_t width, uint16_t height,
                  const NumericRange& range, NumericFormatter formatter,
                  ValueEditorBinding binding, const char* title,
                  uint16_t duration, ValueCallback callback = nullptr,
                  ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm);
    PopupProgress(PixelUI& ui, uint16_t width, uint16_t height,
                  const NumericRange& range, NumericFormatter formatter,
                  int32_t initialValue, const char* title = "",
                  uint16_t duration = 3000);
    ~PopupProgress() = default;

    void drawContent(const PopupContentBounds& bounds) override;
    bool handleContentInput(InputEvent event) override;
};
