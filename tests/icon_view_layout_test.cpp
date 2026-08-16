#include "PixelUI.h"
#include "ui/IconView/IconView.h"

namespace {

uint32_t frameHash(const uint8_t* frame, size_t size) {
    uint32_t hash = 2166136261U;
    for (size_t i = 0; i < size; ++i) {
        hash ^= frame[i];
        hash *= 16777619U;
    }
    return hash;
}

} // namespace

int main() {
    const IconViewLayout standard = calculateIconViewLayout(128, 64);
    if (standard.centerX != 64 || standard.selectorY != 30 ||
        standard.iconY != 18 || standard.progressY != 49 ||
        standard.statusBaseline != 60 || standard.selectedTitleBaseline != 62 ||
        standard.slotPositionsX[0] != 14 ||
        standard.slotPositionsX[1] != 52 ||
        standard.slotPositionsX[2] != 90) return 1;

    const IconViewLayout compact = calculateIconViewLayout(96, 48);
    if (compact.centerX != 48 || compact.selectorY != 22 ||
        compact.iconY != 10 || compact.progressY != 33 ||
        compact.statusBaseline != 44 || compact.selectedTitleBaseline != 46 ||
        compact.slotPositionsX[0] != 6 ||
        compact.slotPositionsX[1] != 36 ||
        compact.slotPositionsX[2] != 66) return 2;

    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    IconView view(ui);
    IconItemList items;
    items.push_back({"One", nullptr});
    items.push_back({"Two", nullptr});
    items.push_back({"Three", nullptr});
    view.setItems(items);
    view.enableProgressBar(true);
    view.enableStatusText(true);
    view.onEnter([]() {});
    ui.heartbeat(700U);
    ui.process();
    display.clearBuffer();
    view.draw();

    const uint8_t* frame = u8g2_GetBufferPtr(display.getU8g2());
    if (frameHash(frame, ui.getDisplayBufferSize()) != 3139118789U) return 3;
    bool selectorBandSet = false;
    bool progressBandSet = false;
    for (int32_t x = 0; x < 128; ++x) {
        selectorBandSet = selectorBandSet ||
            ((frame[x + (standard.selectorY / 8) * 128] &
              (1U << (standard.selectorY % 8))) != 0U);
        progressBandSet = progressBandSet ||
            ((frame[x + (standard.progressY / 8) * 128] &
              (1U << (standard.progressY % 8))) != 0U);
    }
    if (!selectorBandSet || !progressBandSet) return 4;

    return 0;
}
