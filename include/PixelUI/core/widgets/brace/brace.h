#pragma once

#include "../widgets.h"

class Brace : public Widget {
public:
    Brace(PixelUI& ui);
    ~Brace() = default;

    void onLoad() override;
    void onOffload() override;
    void draw() override;

    void setMargin(uint16_t mar_w, uint16_t mar_h) {margin_w_ = mar_w; margin_h_ = mar_h;}
    void setCoordinate(uint16_t coord_x, uint16_t coord_y) {coord_x_ = coord_x; coord_y_=coord_y;}
    void setDrawContentFunction(std::function<void()> func) { contentWithinBrace = func; }
private:
    uint16_t coord_x_ = 0, coord_y_ = 0;
    uint16_t margin_w_ = 0, margin_h_ = 0;
    PixelUI& m_ui;

    std::function<void()> contentWithinBrace;
    // animation related varibles:
    float anim_w = 0;
    float anim_h = 0;
    float anim_x = 0;
    float anim_y = 0;
};