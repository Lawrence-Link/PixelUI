#include "PixelUI.h"
#include "ui/Popup/PopupBase.h"

namespace {

class TestPopup : public PopupBase {
public:
    TestPopup(PixelUI& ui, uint16_t duration)
        : PopupBase(ui, 80, 30, duration) {}

    int shownCount = 0;
    int closingCount = 0;
    int contentInputCount = 0;

protected:
    void drawContent(const PopupContentBounds&) override {}

    bool handleContentInput(InputEvent event) override {
        if (event == InputEvent::RIGHT) {
            ++contentInputCount;
            return true;
        }
        return false;
    }

    void onShown() override { ++shownCount; }
    void onClosing() override { ++closingCount; }
};

} // namespace

int main() {
    U8G2 display;
    PixelUI ui(display);
    TestPopup popup(ui, 1000);

    if (!popup.update(ui.getCurrentTime())) return 1;
    if (ui.activeAnimationCount() != 0U) return 2;

    ui.Heartbeat(300);
    if (!popup.update(ui.getCurrentTime()) || popup.shownCount != 1) return 3;

    ui.Heartbeat(100);
    if (!popup.handleInput(InputEvent::RIGHT) || popup.contentInputCount != 1) return 4;
    if (popup.closingCount != 0) return 5;

    ui.Heartbeat(900);
    if (!popup.update(ui.getCurrentTime()) || popup.closingCount != 0) return 6;

    ui.Heartbeat(101);
    if (!popup.update(ui.getCurrentTime()) || popup.closingCount != 1) return 7;

    ui.Heartbeat(299);
    if (!popup.update(ui.getCurrentTime())) return 8;
    ui.Heartbeat(1);
    if (popup.update(ui.getCurrentTime())) return 9;

    TestPopup inputClosedPopup(ui, 0);
    if (!inputClosedPopup.update(ui.getCurrentTime())) return 10;
    ui.Heartbeat(300);
    if (!inputClosedPopup.update(ui.getCurrentTime())) return 11;
    if (!inputClosedPopup.handleInput(InputEvent::BACK)) return 12;
    if (inputClosedPopup.closingCount != 1) return 13;
    ui.Heartbeat(300);
    if (inputClosedPopup.update(ui.getCurrentTime())) return 14;

    return 0;
}
