// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2025, Lawrence Link

#pragma once

#include "widgets/IWidget.h"

class PixelUI;

class CounterReadingWidget : public IWidget {
public:
    /**
     * @brief Creates the fixed-size counter reading widget.
     * @param ui UI instance that owns the drawing canvas.
     */
    explicit CounterReadingWidget(PixelUI& ui);

    /** @brief Handles loading; this widget currently requires no setup. */
    void onLoad() override;

    /** @brief Handles offloading; this widget currently owns no transient state. */
    void onOffload() override;

private:
    /**
     * @brief Draws the sample reading relative to the widget origin.
     * @param context Render context containing the translated widget origin.
     */
    void drawSelf(const WidgetRenderContext& context) override;

    /** @return The UI canvas used to render this widget. */
    Canvas& display() override;

    PixelUI& ui_;
};
