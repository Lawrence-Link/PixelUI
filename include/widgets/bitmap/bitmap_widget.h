#pragma once

#include "widgets/IWidget.h"

class BitmapWidget : public IWidget {
public:
    BitmapWidget(
        PixelUI& ui,
        int16_t x,
        int16_t y,
        uint16_t width,
        uint16_t height,
        const unsigned char* source = nullptr);

    void onLoad() override {}
    void onOffload() override {}

    void setPosition(int16_t x, int16_t y);
    void setSize(uint16_t width, uint16_t height);
    void setSource(const unsigned char* source) { source_ = source; }

private:
    PixelUI& ui_;
    const unsigned char* source_ = nullptr;

    void drawSelf(const WidgetRenderContext& context) override;
    Canvas& display() override;
};
