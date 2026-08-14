// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2025, Lawrence Link

#pragma once

#include "widgets/IWidget.h"

class PixelUI;

class CounterReadingWidget : public IWidget {
public:
    explicit CounterReadingWidget(PixelUI& ui);

    void onLoad() override;
    void onOffload() override;

private:
    void drawSelf(const WidgetRenderContext& context) override;
    Canvas& display() override;

    PixelUI& ui_;
};
