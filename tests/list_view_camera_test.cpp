#include "PixelUI.h"
#include "ui/ListView/ListView.h"

#include <string.h>

namespace {

class TestListView : public ListView {
public:
    TestListView(PixelUI& ui, ListItem* items, int length)
        : ListView(ui, items, length) {}

    void onLoad() override {}
    void onSave() override {}

    int32_t toggleBoxX(const ListItem& item) const {
        return toggleBoxXFor(item);
    }
};

void finishAnimations(PixelUI& ui) {
    ui.heartbeat(400U);
    ui.process();
}

ValueCallback noOpCallback() {
    return [](int32_t) {};
}

void render(ListView& list, U8G2& display, uint8_t* output) {
    display.clearBuffer();
    list.draw();
    memcpy(output, u8g2_GetBufferPtr(display.getU8g2()), 1024U);
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

    // If the animation store is full, toggling still lands on the stable final state.
    U8G2 fullDisplay;
    u8g2_Setup_ssd1306_128x64_noname_f(
        fullDisplay.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI fullUi(fullDisplay);
    bool toggled = false;
    ListItem fullItems[2] = {{"Back"}, {"Toggle"}};
    fullItems[1].accessory = ListItemAccessory::toggle(toggled);
    TestListView fullList(fullUi, fullItems, 2);
    fullList.onEnter([]() {});
    fullUi.clearAllAnimations();
    fullList.handleInput(LISTVIEW_NAVI_DOWN);
    while (fullUi.activeAnimationCount() < MAX_ANIMATION_COUNT) {
        if (!fullUi.animateCallback(
                0, 1, 1000, EasingType::LINEAR, noOpCallback())) return 7;
    }
    fullList.handleInput(LISTVIEW_NAVI_SELECT);
    if (!toggled) return 8;
    uint8_t fullFrame[1024]{};
    render(fullList, fullDisplay, fullFrame);
    toggled = false;
    if (fullList.toggleBoxX(fullItems[1]) != 0) return 9;

    U8G2 referenceDisplay;
    u8g2_Setup_ssd1306_128x64_noname_f(
        referenceDisplay.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI referenceUi(referenceDisplay);
    bool referenceToggle = true;
    ListItem referenceItems[2] = {{"Back"}, {"Toggle"}};
    referenceItems[1].accessory = ListItemAccessory::toggle(referenceToggle);
    TestListView referenceList(referenceUi, referenceItems, 2);
    referenceList.onEnter([]() {});
    referenceUi.clearAllAnimations();
    referenceList.handleInput(LISTVIEW_NAVI_DOWN);
    referenceUi.clearAllAnimations();
    uint8_t referenceFrame[1024]{};
    render(referenceList, referenceDisplay, referenceFrame);
    if (memcmp(fullFrame, referenceFrame, sizeof(fullFrame)) != 0) return 10;

    // Parent and child toggles at row 1 have different stable identities.
    U8G2 nestedDisplay;
    u8g2_Setup_ssd1306_128x64_noname_f(
        nestedDisplay.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI nestedUi(nestedDisplay);
    bool parentToggle = false;
    bool childToggle = false;
    ListItem children[2] = {{"Back"}, {"Child toggle"}};
    children[1].accessory = ListItemAccessory::toggle(childToggle);
    ListItem parents[3] = {{"Back"}, {"Parent toggle"}, {"Submenu"}};
    parents[1].accessory = ListItemAccessory::toggle(parentToggle);
    parents[2].nextList = children;
    parents[2].nextListLength = 2;
    TestListView nestedList(nestedUi, parents, 3);
    nestedList.onEnter([]() {});
    nestedUi.clearAllAnimations();
    nestedList.handleInput(LISTVIEW_NAVI_DOWN);
    nestedList.handleInput(LISTVIEW_NAVI_SELECT);
    nestedUi.heartbeat(100U);
    nestedUi.process();
    nestedList.handleInput(LISTVIEW_NAVI_DOWN);
    nestedList.handleInput(LISTVIEW_NAVI_SELECT);
    nestedUi.heartbeat(400U);
    nestedUi.process();
    parentToggle = false;
    if (nestedList.toggleBoxX(parents[1]) != 0) return 11;
    nestedList.handleInput(LISTVIEW_NAVI_DOWN);
    nestedUi.clearAllAnimations();
    if (nestedList.toggleBoxX(children[1]) != 0 ||
        nestedList.toggleBoxX(parents[1]) != 0) return 12;

    return 0;
}
