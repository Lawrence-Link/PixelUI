#include "PixelUI.h"

namespace {

class TestWidget : public IWidget {
public:
    TestWidget() {
        setFocusable(true);
        setFocusBox({10, 12, 20, 8});
    }

    void onLoad() override {}
    void onOffload() override {}

    bool onSelect() override {
        ++selectCount;
        return true;
    }

    bool handleEvent(InputEvent) override {
        ++inputCount;
        return true;
    }

    void onActivate(uint32_t currentTime) override {
        IWidget::onActivate(currentTime);
        ++activateCount;
    }

    void onDeactivate() override {
        IWidget::onDeactivate();
        ++deactivateCount;
    }

    int selectCount = 0;
    int inputCount = 0;
    int activateCount = 0;
    int deactivateCount = 0;

private:
    void drawSelf(const WidgetRenderContext&) override {}
    Canvas& display() override { return ui_->getCanvas(); }

public:
    void attach(PixelUI& ui) { ui_ = &ui; }

private:
    PixelUI* ui_ = nullptr;
};

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    TestWidget widget;
    widget.attach(ui);
    ui.addWidgetToFocusManager(&widget);

    ui.handleInput(InputEvent::RIGHT);
    if (ui.activeAnimationCount() != 4U) return 1;

    ui.Heartbeat(100);
    ui.renderer();
    if (ui.activeAnimationCount() != 0U) return 2;

    ui.handleInput(InputEvent::SELECT);
    if (!widget.isActive() || widget.selectCount != 1 || widget.activateCount != 1) return 3;

    ui.handleInput(InputEvent::BACK);
    if (widget.isActive() || widget.inputCount != 1 || widget.deactivateCount != 1) return 4;

    ui.Heartbeat(2501);
    ui.renderer();
    if (ui.activeAnimationCount() != 4U) return 5;

    ui.Heartbeat(100);
    ui.renderer();
    if (ui.activeAnimationCount() != 0U) return 6;

    ui.handleInput(InputEvent::SELECT);
    if (widget.selectCount != 1 || widget.activateCount != 1) return 7;

    ui.handleInput(InputEvent::RIGHT);
    ui.Heartbeat(100);
    ui.renderer();
    ui.handleInput(InputEvent::SELECT);
    if (!widget.isActive() || widget.activateCount != 2) return 8;

    ui.clearFocusManager();
    if (ui.getFocusedWidgetCount() != 0U || widget.isActive() || widget.deactivateCount != 2) return 9;
    ui.handleInput(InputEvent::SELECT);
    if (widget.selectCount != 2) return 10;

    return 0;
}
