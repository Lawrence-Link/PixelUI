#include "PixelUI.h"
#include "ui/IconView/IconView.h"

#include <string.h>

namespace {

uint32_t frameHash(const uint8_t* frame, size_t size) {
    uint32_t hash = 2166136261U;
    for (size_t i = 0; i < size; ++i) {
        hash ^= frame[i];
        hash *= 16777619U;
    }
    return hash;
}

bool titlePixelsStayCentered(const uint8_t* frame) {
    bool found = false;
    for (int32_t y = 56; y <= 62; ++y) {
        for (int32_t x = 0; x < 128; ++x) {
            const size_t index = static_cast<size_t>(x) +
                                 static_cast<size_t>(y / 8) * 128U;
            if ((frame[index] & static_cast<uint8_t>(1U << (y & 7))) == 0U) {
                continue;
            }
            if (x < 60 || x > 67) return false;
            found = true;
        }
    }
    return found;
}

bool titlePixelsStayInsideViewport(
    const uint8_t* frame, int32_t left, int32_t right) {
    for (int32_t y = 56; y <= 62; ++y) {
        for (int32_t x = 0; x < 128; ++x) {
            const size_t index = static_cast<size_t>(x) +
                                 static_cast<size_t>(y / 8) * 128U;
            if ((frame[index] & static_cast<uint8_t>(1U << (y & 7))) != 0U &&
                (x < left || x >= right)) {
                return false;
            }
        }
    }
    return true;
}

} // namespace

int main() {
    const IconViewLayout standard = calculateIconViewLayout(128, 64);
    if (standard.centerX != 64 || standard.selectorY != 30 ||
        standard.iconY != 18 || standard.progressY != 49 ||
        standard.statusBaseline != 60 || standard.selectedTitleBaseline != 62 ||
        standard.selectedTitleX != 15 || standard.selectedTitleWidth != 98 ||
        standard.slotPositionsX[0] != 14 ||
        standard.slotPositionsX[1] != 52 ||
        standard.slotPositionsX[2] != 90) return 1;

    const IconViewLayout compact = calculateIconViewLayout(96, 48);
    if (compact.centerX != 48 || compact.selectorY != 22 ||
        compact.iconY != 10 || compact.progressY != 33 ||
        compact.statusBaseline != 44 || compact.selectedTitleBaseline != 46 ||
        compact.selectedTitleX != 15 || compact.selectedTitleWidth != 66 ||
        compact.slotPositionsX[0] != 6 ||
        compact.slotPositionsX[1] != 36 ||
        compact.slotPositionsX[2] != 66) return 2;

    const IconViewLayout narrow = calculateIconViewLayout(20, 16);
    if (narrow.selectedTitleX != 0 || narrow.selectedTitleWidth != 20) return 24;

    if (TextAlignHelper::calcAlignedOffset(
            98, 4, TextAlignX::Center,
            TextOverflowPlacement::PinToLeadingEdge) != 47) return 20;
    if (TextAlignHelper::calcAlignedOffset(
            98, 120, TextAlignX::Center,
            TextOverflowPlacement::PinToLeadingEdge) != 0) return 21;
    if (TextAlignHelper::calcAlignedOffset(
            98, 120, TextAlignX::Center) != -11) return 22;

    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    ui.begin();
    static_assert(sizeof(IconView<1>) < sizeof(IconView<3>));
    IconView<3> view(ui);
    IconItemList<3> items;
    items.push_back({"One", nullptr});
    items.push_back({"Two", nullptr});
    items.push_back({"Three", nullptr});
    if (!view.setItems(items)) return 3;
    IconItemList<4> tooManyItems;
    tooManyItems.push_back({"One", nullptr});
    tooManyItems.push_back({"Two", nullptr});
    tooManyItems.push_back({"Three", nullptr});
    tooManyItems.push_back({"Four", nullptr});
    if (view.setItems(tooManyItems)) return 4;
    view.enableProgressBar(true);
    view.enableStatusText(true);
    view.onEnter([]() {});
    ui.heartbeat(700U);
    ui.process();
    display.clearBuffer();
    view.draw();

    const uint8_t* frame = u8g2_GetBufferPtr(display.getU8g2());
    if (frameHash(frame, ui.getDisplayBufferSize()) != 3139118789U) return 5;
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
    if (!selectorBandSet || !progressBandSet) return 6;

    U8G2 titleDisplay;
    u8g2_Setup_ssd1306_128x64_noname_f(
        titleDisplay.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI titleUi(titleDisplay);
    titleUi.begin();
    IconView<2> titleView(titleUi, u8g2_font_4x6_tr);
    IconItemList<2> titleItems;
    titleItems.push_back({"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", nullptr});
    titleItems.push_back({"A", nullptr});
    if (!titleView.setItems(titleItems)) return 7;
    titleView.enableSelectedItemTitle(true);
    titleView.onEnter([]() {});

    titleUi.tickFromISR(800U);
    titleUi.process();
#if PIXELUI_USE_LABEL_SCROLL
    uint8_t initialTitleFrame[1024] = {};
    uint8_t scrolledTitleFrame[1024] = {};
    titleDisplay.clearBuffer();
    titleView.draw();
    memcpy(initialTitleFrame, titleDisplay.getBufferPtr(), sizeof(initialTitleFrame));
    if (!titlePixelsStayInsideViewport(
            initialTitleFrame, standard.selectedTitleX,
            standard.selectedTitleX + standard.selectedTitleWidth)) return 23;
    if (!titleUi.needsHeartbeat()) return 8;

    titleUi.tickFromISR(50U);
    titleUi.process();
    titleDisplay.clearBuffer();
    titleView.draw();
    memcpy(scrolledTitleFrame, titleDisplay.getBufferPtr(), sizeof(scrolledTitleFrame));
    if (memcmp(initialTitleFrame, scrolledTitleFrame,
               sizeof(initialTitleFrame)) == 0) return 9;
#endif

    if (!titleView.handleInput(ICONVIEW_NAVI_RIGHT)) return 10;
    titleUi.tickFromISR(550U);
    titleUi.process();
    titleDisplay.clearBuffer();
    titleView.draw();
    if (!titlePixelsStayCentered(titleDisplay.getBufferPtr())) return 11;
    if (titleUi.needsHeartbeat()) return 12;

    if (!titleView.handleInput(ICONVIEW_NAVI_LEFT) ||
        !titleUi.needsHeartbeat()) return 13;
    titleView.onPause();

    return 0;
}
