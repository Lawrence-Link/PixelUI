#include "PixelUI.h"
#include "widgets/brace/brace.h"

namespace {

class ProbeWidget : public IWidget {
public:
    ProbeWidget(PixelUI& ui, int32_t x, int32_t y, int32_t w, int32_t h, int id = 0)
        : ui_(ui), id_(id) {
        setWidgetBounds({x, y, w, h});
        setFocusBox({x, y, w, h});
    }

    void onLoad() override {}
    void onOffload() override {}
    bool onSelect() override {
        if (selectionLog_ && selectionCount_) selectionLog_[(*selectionCount_)++] = id_;
        return false;
    }

    void setSelectionLog(int* log, size_t& count) {
        selectionLog_ = log;
        selectionCount_ = &count;
    }

    void setDrawLog(int* log, size_t& count) {
        drawLog_ = log;
        drawCount_ = &count;
    }

    void configureFocusInsets(const FocusInsets& insets) { setFocusInsets(insets); }
    void setBounds(const FocusBox& bounds) { setWidgetBounds(bounds); }

    int32_t screenX = -1;
    int32_t screenY = -1;
    FocusBox receivedClip = {0, 0, 0, 0};

private:
    void drawSelf(const WidgetRenderContext& context) override {
        if (drawLog_ && drawCount_) drawLog_[(*drawCount_)++] = id_;
        const FocusBox bounds = getLocalBounds();
        screenX = context.originX + bounds.x;
        screenY = context.originY + bounds.y;
        receivedClip = context.clip;
    }

    Canvas& display() override { return ui_.getCanvas(); }

    PixelUI& ui_;
    int id_ = 0;
    int* selectionLog_ = nullptr;
    size_t* selectionCount_ = nullptr;
    int* drawLog_ = nullptr;
    size_t* drawCount_ = nullptr;
};

class LayeredProbe : public IWidget {
public:
    LayeredProbe(
        PixelUI& ui,
        const FocusBox& bounds,
        const FocusBox& childrenClip,
        int* drawLog,
        size_t& drawCount)
        : ui_(ui), childrenClip_(childrenClip), drawLog_(drawLog), drawCount_(drawCount) {
        setWidgetBounds(bounds);
    }

    void onLoad() override {}
    void onOffload() override {}

private:
    PixelUI& ui_;
    FocusBox childrenClip_;
    int* drawLog_;
    size_t& drawCount_;

    void drawSelf(const WidgetRenderContext&) override { drawLog_[drawCount_++] = 1; }
    void drawOverlay(const WidgetRenderContext&) override { drawLog_[drawCount_++] = 3; }
    FocusBox getChildrenClipBounds() const override { return childrenClip_; }
    Canvas& display() override { return ui_.getCanvas(); }
};

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);

    ProbeWidget root(ui, 10, 8, 30, 20);
    ProbeWidget first(ui, 3, 4, 12, 10);
    ProbeWidget second(ui, 18, 2, 20, 12);
    ProbeWidget grandchild(ui, 2, 3, 8, 8);

    if (!root.addChild(first) || !root.addChild(second) || !first.addChild(grandchild)) return 1;
    if (root.firstChild() != &first || root.lastChild() != &second) return 2;
    if (first.nextSibling() != &second || second.previousSibling() != &first) return 3;
    if (root.setParent(&grandchild)) return 4;

    root.draw();
    if (root.screenX != 10 || root.screenY != 8) return 5;
    if (first.screenX != 13 || first.screenY != 12) return 6;
    if (grandchild.screenX != 15 || grandchild.screenY != 15) return 7;
    if (!(first.receivedClip == FocusBox{10, 8, 30, 20})) return 8;
    if (!(grandchild.receivedClip == FocusBox{13, 12, 12, 10})) return 9;
    if (!(second.receivedClip == FocusBox{10, 8, 30, 20})) return 10;

    root.setVisible(false);
    first.screenX = -1;
    root.draw();
    if (first.screenX != -1) return 11;
    root.setVisible(true);

    if (!second.addChild(grandchild)) return 12;
    if (grandchild.parent() != &second || first.firstChild() != nullptr) return 13;
    if (!second.removeChild(grandchild) || grandchild.parent() != nullptr) return 14;

    {
        ProbeWidget temporary(ui, 1, 1, 2, 2);
        if (!root.addChild(temporary)) return 15;
    }
    if (root.lastChild() != &second || second.nextSibling() != nullptr) return 16;

    ProbeWidget focusRoot(ui, 0, 0, 128, 64);
    focusRoot.setFocusable(false);
    ProbeWidget child1(ui, 1, 1, 5, 5, 1);
    ProbeWidget child2(ui, 7, 1, 5, 5, 2);
    ProbeWidget child3(ui, 13, 1, 5, 5, 3);
    ProbeWidget child4(ui, 19, 1, 5, 5, 4);
    ProbeWidget child5(ui, 25, 1, 5, 5, 5);
    ProbeWidget child6(ui, 31, 1, 5, 5, 6);
    ProbeWidget child7(ui, 37, 1, 5, 5, 7);
    ProbeWidget* children[] = {&child1, &child2, &child3, &child4, &child5, &child6, &child7};
    int selectionLog[8] = {};
    size_t selectionCount = 0;
    for (ProbeWidget* child : children) {
        child->setFocusable(true);
        child->setSelectionLog(selectionLog, selectionCount);
        if (!focusRoot.addChild(*child)) return 17;
    }

    ui.addWidgetToFocusManager(&focusRoot);
    if (ui.getFocusedWidgetCount() != 7U) return 18;
    ProbeWidget unregisteredParent(ui, 0, 0, 128, 64);
    if (unregisteredParent.addChild(focusRoot)) return 19;
    for (int expected = 1; expected <= 7; ++expected) {
        ui.handleInput(InputEvent::RIGHT);
        ui.heartbeat(100);
        ui.renderer();
        ui.handleInput(InputEvent::SELECT);
        if (selectionLog[expected - 1] != expected) return 20;
    }

    child4.setVisible(false);
    if (ui.getFocusedWidgetCount() != 6U) return 21;
    focusRoot.setEnabled(false);
    if (ui.getFocusedWidgetCount() != 0U) return 22;
    focusRoot.setEnabled(true);
    child4.setVisible(true);
    ui.handleInput(InputEvent::RIGHT);
    ui.heartbeat(100);
    ui.renderer();
    if (!focusRoot.removeChild(child1)) return 23;
    ui.handleInput(InputEvent::SELECT);
    if (selectionCount != 7U) return 24;
    ui.clearFocusManager();

    {
        ProbeWidget temporaryRoot(ui, 0, 0, 10, 10);
        temporaryRoot.setFocusable(true);
        ui.addWidgetToFocusManager(&temporaryRoot);
        if (ui.getFocusedWidgetCount() != 1U) return 25;
    }
    if (ui.getFocusedWidgetCount() != 0U) return 26;

    ProbeWidget detachRoot(ui, 0, 0, 20, 20);
    ProbeWidget detachChild(ui, 1, 1, 5, 5, 8);
    detachChild.setFocusable(true);
    detachChild.setSelectionLog(selectionLog, selectionCount);
    if (!detachRoot.addChild(detachChild)) return 27;
    FocusManager firstManager(ui);
    FocusManager secondManager(ui);
    if (!firstManager.addWidget(&detachRoot)) return 28;
    firstManager.moveNext();
    ui.heartbeat(100);
    ui.renderer();
    firstManager.selectCurrent();
    if (selectionCount != 8U) return 29;

    // A non-root remains owned by its registered tree's observer.
    firstManager.removeWidget(&detachChild);
    if (secondManager.addWidget(&detachChild)) return 30;

    detachRoot.removeAllChildren();
    if (detachChild.parent()) return 31;
    firstManager.selectCurrent();
    if (selectionCount != 8U) return 32;

    ProbeWidget clippedRoot(ui, 0, 0, 0, 0);
    ProbeWidget clippedChild(ui, 1, 1, 5, 5);
    if (!clippedRoot.addChild(clippedChild)) return 33;
    clippedRoot.draw();
    if (clippedChild.screenX != -1) return 34;

    ProbeWidget contextualRoot(ui, 45, 22, 20, 20);
    ProbeWidget contextualChild(ui, 2, 2, 5, 5);
    if (!contextualRoot.addChild(contextualChild)) return 35;
    const WidgetRenderContext popupContext{0, 0, {40, 20, 30, 18}};
    contextualRoot.draw(popupContext);
    if (!(contextualRoot.receivedClip == popupContext.clip)) return 36;
    if (!(contextualChild.receivedClip == FocusBox{45, 22, 20, 16})) return 37;

    int drawLog[3] = {};
    size_t drawCount = 0;
    LayeredProbe layered(ui, {10, 8, 30, 20}, {12, 10, 6, 7}, drawLog, drawCount);
    ProbeWidget layeredChild(ui, 1, 1, 4, 4, 2);
    layeredChild.setDrawLog(drawLog, drawCount);
    if (!layered.addChild(layeredChild)) return 38;
    layered.draw();
    if (drawCount != 3U || drawLog[0] != 1 || drawLog[1] != 2 || drawLog[2] != 3) return 39;
    if (!(layeredChild.receivedClip == FocusBox{12, 10, 6, 7})) return 40;

    Brace animatedBrace(ui, 5, 6, 20, 10);
    ProbeWidget braceChild(ui, 1, 1, 4, 4);
    if (!animatedBrace.addChild(braceChild)) return 41;
    animatedBrace.draw();
    if (braceChild.screenX != -1) return 42;

    bool braceSelected = false;
    animatedBrace.setFocusable(true);
    animatedBrace.setCallback([&braceSelected]() { braceSelected = true; });
    if (animatedBrace.onSelect() || !braceSelected || !animatedBrace.isFocusable()) return 43;

    animatedBrace.onLoad();
    ui.heartbeat(600);
    ui.process();
    animatedBrace.draw();
    if (braceChild.screenX != 6 || braceChild.screenY != 7) return 44;
    if (!(braceChild.receivedClip == FocusBox{5, 6, 20, 10})) return 45;

    ProbeWidget insetWidget(ui, 10, 12, 20, 8);
    if (!(insetWidget.getFocusInsets() == FocusInsets{})) return 46;
    if (!(insetWidget.getFocusBox() == FocusBox{10, 12, 20, 8})) return 47;

    insetWidget.configureFocusInsets({1, 2, 3, 4});
    if (!(insetWidget.getFocusBox() == FocusBox{11, 14, 16, 2})) return 48;
    insetWidget.setBounds({20, 30, 4, 3});
    if (!(insetWidget.getFocusBox() == FocusBox{21, 32, 0, 0})) return 49;

    insetWidget.configureFocusInsets({-1, -2, -3, -4});
    if (!(insetWidget.getFocusBox() == FocusBox{19, 28, 8, 9})) return 50;

    ProbeWidget insetParent(ui, 7, 9, 40, 20);
    if (!insetParent.addChild(insetWidget)) return 51;
    if (!(insetWidget.getFocusBox() == FocusBox{26, 37, 8, 9})) return 52;

    if (!(animatedBrace.getFocusInsets() == FocusInsets{1, 1, 0, 0})) return 53;
    if (!(animatedBrace.getFocusBox() == FocusBox{6, 7, 19, 9})) return 54;

    return 0;
}
