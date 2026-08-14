// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2025, Lawrence Link

#include "counter_reading_widget.h"

#include "PixelUI.h"

CounterReadingWidget::CounterReadingWidget(PixelUI& ui) : ui_(ui) {
    setWidgetBounds({0, 0, 56, 18});
}

void CounterReadingWidget::onLoad() {}

void CounterReadingWidget::onOffload() {}

void CounterReadingWidget::drawSelf(const WidgetRenderContext& context) {
    Canvas& canvas = ui_.getCanvas();
    canvas.setFont(u8g2_font_5x7_tr);
    canvas.drawStr(context.originX + 27, context.originY + 9, "10.00");
    canvas.drawStr(context.originX + 28, context.originY + 16, "uSv/h");
    canvas.drawRBox(context.originX + 5, context.originY + 5, 20, 10, 2);
    canvas.setDrawColor(0);
    canvas.drawStr(context.originX + 8, context.originY + 13, "Max");
    canvas.setDrawColor(1);
}

Canvas& CounterReadingWidget::display() {
    return ui_.getCanvas();
}
