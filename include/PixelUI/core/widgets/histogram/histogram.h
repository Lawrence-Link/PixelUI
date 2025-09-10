#pragma once

#include "../widgets.h"

class Histogram : public Widget {
public:
    Histogram(PixelUI& ui);
    ~Histogram() = default;

    void onLoad() override;
    void onOffload() override;
    void draw() override;

    void setMargin(uint16_t mar_w, uint16_t mar_h) { margin_w_ = mar_w; margin_h_ = mar_h; }
    void setCoordinate(uint16_t coord_x, uint16_t coord_y) { coord_x_ = coord_x; coord_y_ = coord_y; }
    
    /**
     * @brief Sets the data for the histogram from a circular buffer.
     * @param data_ptr Pointer to the float array (circular buffer).
     * @param data_size The total size of the circular buffer.
     * @param head_index The current head index of the circular buffer.
     */
    void setData(float* data_ptr, uint16_t data_size, uint16_t head_index);

private:
    uint16_t coord_x_ = 0, coord_y_ = 0;
    uint16_t margin_w_ = 0, margin_h_ = 0;
    PixelUI& m_ui;

    float* m_data_ptr = nullptr;     // Pointer to the external circular buffer
    uint16_t m_data_size = 0;        // Total size of the circular buffer
    uint16_t m_head_index = 0;       // Current head index of the circular buffer

    // animation related variables:
    float anim_w = 0;
    float anim_h = 0;
    float anim_x = 0;
    float anim_y = 0;
};