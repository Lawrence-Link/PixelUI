#include "PixelUI.h"
#include "ui/ListView/ListView.h"

namespace {

class TestListView : public ListView {
public:
    TestListView(PixelUI& ui, ListItem* items, int length)
        : ListView(ui, items, length) {}

    void onLoad() override {}
    void onSave() override {}
};

void finishAnimations(PixelUI& ui) {
    ui.heartbeat(400U);
    ui.process();
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    ListItem items[6] = {
        {"Back"}, {"One"}, {"Two"}, {"Three"}, {"Four"}, {"Five"}
    };
    TestListView list(ui, items, 6);
    list.onEnter([]() {});
    ui.clearAllAnimations();

    for (int i = 0; i < 3; ++i) {
        if (!list.handleInput(LISTVIEW_NAVI_DOWN)) return 1;
        finishAnimations(ui);
    }
    if (ui.getCanvas().camera().storedY() != 0) return 2;

    if (!list.handleInput(LISTVIEW_NAVI_DOWN)) return 3;
    finishAnimations(ui);
    const int32_t oneRowOffset = ui.getCanvas().camera().storedY();
    if (oneRowOffset <= 0) return 4;

    for (int i = 0; i < 4; ++i) {
        if (!list.handleInput(LISTVIEW_NAVI_UP)) return 5;
        finishAnimations(ui);
    }
    if (ui.getCanvas().camera().storedY() != 0) return 6;

    return 0;
}
