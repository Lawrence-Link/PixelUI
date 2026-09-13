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

bool framesEqualFromX(
    const uint8_t* first, const uint8_t* second, int32_t startX) {
    for (int32_t page = 0; page < 8; ++page) {
        for (int32_t x = startX; x < 128; ++x) {
            const size_t index = static_cast<size_t>(x + page * 128);
            if (first[index] != second[index]) return false;
        }
    }
    return true;
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    bool cameraTitleToggle = false;
    ListItem items[6] = {
        {"Back"}, {"One"}, {"Two"}, {"ABCDEFGHIJKLMNOPQRSTUVWXYZ12"},
        {"Four"}, {"Five"}
    };
    items[3].accessory = ListItemAccessory::toggle(cameraTitleToggle);
    TestListView list(ui, items, 6);
    list.onEnter([]() {});
    ui.clearAllAnimations();

    for (int i = 0; i < 3; ++i) {
        if (!list.handleInput(LISTVIEW_NAVI_DOWN)) return 1;
        finishAnimations(ui);
    }
    if (ui.getCanvas().camera().storedY() != 0) return 2;
#if PIXELUI_USE_LABEL_SCROLL
    ui.clearAllAnimations();
    if (!ui.getCanvas().camera().setY(10)) return 25;
    uint8_t cameraTitleFrame[1024]{};
    uint8_t scrolledCameraTitleFrame[1024]{};
    render(list, display, cameraTitleFrame);
    ui.tickFromISR(800U);
    ui.process();
    ui.tickFromISR(250U);
    ui.process();
    render(list, display, scrolledCameraTitleFrame);
    if (memcmp(cameraTitleFrame, scrolledCameraTitleFrame,
               sizeof(cameraTitleFrame)) == 0) return 26;
    ui.getCanvas().camera().setY(0);
#endif

    if (!list.handleInput(LISTVIEW_NAVI_DOWN)) return 3;
    finishAnimations(ui);
    const int32_t oneRowOffset = ui.getCanvas().camera().storedY();
    if (oneRowOffset <= 0) return 4;
    if (!list.handleInput(InputEvent::UP) ||
        !list.handleInput(InputEvent::DOWN)) return 13;

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

    U8G2 titleDisplay;
    u8g2_Setup_ssd1306_128x64_noname_f(
        titleDisplay.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI titleUi(titleDisplay);
    bool titleToggle = false;
    ListItem titleItems[3] = {
        {"Back"}, {"ABCDEFGHIJKLMNOPQRSTUVWXYZ12"}, {"Short"}
    };
    titleItems[1].accessory = ListItemAccessory::toggle(titleToggle);
    TestListView titleList(titleUi, titleItems, 3);
    titleList.onEnter([]() {});
    titleUi.clearAllAnimations();
    if (!titleList.handleInput(LISTVIEW_NAVI_DOWN)) return 14;
    titleUi.clearAllAnimations();
#if PIXELUI_USE_LABEL_SCROLL
    if (!titleUi.needsHeartbeat()) return 15;
#endif

    uint8_t initialTitleFrame[1024]{};
    uint8_t pausedTitleFrame[1024]{};
    uint8_t scrolledTitleFrame[1024]{};
    render(titleList, titleDisplay, initialTitleFrame);
    titleUi.tickFromISR(800U);
    titleUi.process();
    render(titleList, titleDisplay, pausedTitleFrame);
    if (memcmp(initialTitleFrame, pausedTitleFrame,
               sizeof(initialTitleFrame)) != 0) return 16;
#if PIXELUI_USE_LABEL_SCROLL
    titleUi.tickFromISR(50U);
    titleUi.process();
    render(titleList, titleDisplay, scrolledTitleFrame);
    if (memcmp(initialTitleFrame, scrolledTitleFrame,
               sizeof(initialTitleFrame)) == 0) return 17;
#endif

    U8G2 clippedReferenceDisplay;
    u8g2_Setup_ssd1306_128x64_noname_f(
        clippedReferenceDisplay.getU8g2(), U8G2_R0,
        u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI clippedReferenceUi(clippedReferenceDisplay);
    bool clippedReferenceToggle = false;
    ListItem clippedReferenceItems[3] = {
        {"Back"}, {""}, {"Short"}
    };
    clippedReferenceItems[1].accessory =
        ListItemAccessory::toggle(clippedReferenceToggle);
    TestListView clippedReferenceList(
        clippedReferenceUi, clippedReferenceItems, 3);
    clippedReferenceList.onEnter([]() {});
    clippedReferenceUi.clearAllAnimations();
    if (!clippedReferenceList.handleInput(LISTVIEW_NAVI_DOWN)) return 18;
    clippedReferenceUi.clearAllAnimations();
    uint8_t clippedReferenceFrame[1024]{};
    render(clippedReferenceList, clippedReferenceDisplay,
           clippedReferenceFrame);
    constexpr int32_t TOGGLE_TITLE_RIGHT = 128 - 42 - 3;
    if (!framesEqualFromX(
            initialTitleFrame, clippedReferenceFrame,
            TOGGLE_TITLE_RIGHT)) return 19;

    if (!titleList.handleInput(LISTVIEW_NAVI_DOWN)) return 20;
    titleUi.clearAllAnimations();
    if (titleUi.needsHeartbeat()) return 21;
    if (!titleList.handleInput(LISTVIEW_NAVI_UP)) return 22;
    titleUi.clearAllAnimations();
#if PIXELUI_USE_LABEL_SCROLL
    if (!titleUi.needsHeartbeat()) return 23;
#endif
    titleList.onPause();
    if (titleUi.needsHeartbeat()) return 24;

    return 0;
}
