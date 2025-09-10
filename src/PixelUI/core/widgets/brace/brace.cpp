#include "PixelUI/core/widgets/brace/brace.h"

Brace::Brace(PixelUI& ui) : m_ui(ui) {
    onLoad();
};

void Brace::onLoad() {
    m_ui.animate(anim_w, margin_w_, 550, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
    m_ui.animate(anim_h, margin_h_, 600, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
}

void Brace::onOffload() {

}

void Brace::draw() {
    U8G2& u8g2 = m_ui.getU8G2();

    int half_width = anim_w / 2;
    int half_height = anim_h / 2;

    int clip_x = coord_x_ - half_width;
    int clip_y = coord_y_ - half_height;
    int clip_width = anim_w;
    int clip_height = anim_h;

    // Set the clipping window based on the current brace dimensions
    u8g2.setClipWindow(
        clip_x, 
        clip_y, 
        clip_x + clip_width,   // 右下角X
        clip_y + clip_height   // 右下角Y
    );

    if (contentWithinBrace) {
        contentWithinBrace();
    }
    u8g2.setMaxClipWindow();

    // Top left corner
    u8g2.drawLine(coord_x_ - half_width, coord_y_ - half_height, coord_x_ - half_width + 4, coord_y_ - half_height);
    u8g2.drawLine(coord_x_ - half_width, coord_y_ - half_height, coord_x_ - half_width, coord_y_ - half_height + 4);

    // Top right corner
    u8g2.drawLine(coord_x_ + half_width, coord_y_ - half_height, coord_x_ + half_width - 4, coord_y_ - half_height);
    u8g2.drawLine(coord_x_ + half_width, coord_y_ - half_height, coord_x_ + half_width, coord_y_ - half_height + 4);

    // Bottom left corner
    u8g2.drawLine(coord_x_ - half_width, coord_y_ + half_height, coord_x_ - half_width + 4, coord_y_ + half_height);
    u8g2.drawLine(coord_x_ - half_width, coord_y_ + half_height, coord_x_ - half_width, coord_y_ + half_height - 4);

    // Bottom right corner
    u8g2.drawLine(coord_x_ + half_width, coord_y_ + half_height, coord_x_ + half_width - 4, coord_y_ + half_height);
    u8g2.drawLine(coord_x_ + half_width, coord_y_ + half_height, coord_x_ + half_width, coord_y_ + half_height - 4);
}