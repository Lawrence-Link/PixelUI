#include "PixelUI.h"
#include "core/ViewManager/ViewManager.h"
#include "core/app/app_system.h"

namespace {

struct AppOptions {
    int32_t contentHeight = 71;
    bool consumeDown = false;
};

class ScrollableApplication : public IApplication {
public:
    ScrollableApplication(PixelUI& ui, void* parameters)
        : IApplication(true), ui_(ui), options_(*static_cast<AppOptions*>(parameters)) {}

    void draw() override {
        Canvas& canvas = ui_.getCanvas();
        canvas.setContentHeight(options_.contentHeight);
        canvas.drawPixel(3, 5);
        canvas.drawPixel(3, 70);
        ui_.getU8G2().drawPixel(4, 5);
    }

    bool handleInput(InputEvent event) override {
        return options_.consumeDown && event == InputEvent::DOWN;
    }

    void onEnter(ExitCallback callback) override {
        IApplication::onEnter(callback);
    }

private:
    PixelUI& ui_;
    AppOptions& options_;
};

class FixedApplication : public IApplication {
public:
    FixedApplication(PixelUI&, void*) {}
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }
};

class EnterTransitionApplication : public IApplication {
public:
    EnterTransitionApplication(PixelUI& ui, void*) : ui_(ui) {
        setEnterTransitionEnabled(true);
    }

    void draw() override {
        ui_.getCanvas().drawPixel(5, 5);
        ui_.getU8G2().drawPixel(4, 5);
    }

    bool handleInput(InputEvent) override { return false; }

private:
    PixelUI& ui_;
};

bool pixelIsSet(U8G2& display, int32_t x, int32_t y) {
    const uint8_t* buffer = display.getBufferPtr();
    const size_t index = static_cast<size_t>(x) +
                         static_cast<size_t>(y / 8) * display.getDisplayWidth();
    return (buffer[index] & static_cast<uint8_t>(1U << (y & 7))) != 0U;
}

} // namespace

int main() {
    U8G2 display;
    u8g2_Setup_ssd1306_128x64_noname_f(
        display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
    PixelUI ui(display);
    if (ui.getDisplayWidth() != 128U || ui.getDisplayHeight() != 64U ||
        ui.getDisplayBufferSize() != 1024U) return 14;
    ViewManager& manager = *ui.getViewManagerPtr();
    AppOptions options;
    const AppItem scrollable =
        AppItem::make<ScrollableApplication>("Scrollable", nullptr);
    const AppItem fixed = AppItem::make<FixedApplication>("Fixed", nullptr);

    if (manager.launch(scrollable, &options) != ViewManager::LaunchResult::Ok) return 1;
    if (!ui.getCanvas().camera().isEnabled()) return 2;
    if (!ui.renderer()) return 3;
    if (ui.getCanvas().camera().contentHeight() != 71) return 4;
    if (!pixelIsSet(display, 3, 5) || !pixelIsSet(display, 4, 5) ||
        pixelIsSet(display, 3, 63)) return 5;

    ui.getCanvas().camera().setX(-5);
    ui.markDirty();
    if (!ui.renderer() || pixelIsSet(display, 3, 5) ||
        !pixelIsSet(display, 8, 5)) return 15;
    ui.getCanvas().camera().setX(0);

    ui.handleInput(InputEvent::DOWN);
    if (ui.getCanvas().camera().storedY() != 7) return 6;
    if (!ui.renderer()) return 7;
    if (!pixelIsSet(display, 3, 63) || !pixelIsSet(display, 4, 5)) return 8;

    options.consumeDown = true;
    ui.handleInput(InputEvent::DOWN);
    if (ui.getCanvas().camera().storedY() != 7) return 9;

    if (manager.launch(fixed, nullptr) != ViewManager::LaunchResult::Ok) return 10;
    if (ui.getCanvas().camera().isEnabled() || ui.getCanvas().camera().storedY() != 0) return 11;
    if (!manager.pop()) return 12;
    if (!ui.getCanvas().camera().isEnabled() ||
        ui.getCanvas().camera().storedY() != 7 ||
        ui.getCanvas().camera().contentHeight() != 71) return 13;

    {
        U8G2 transitionDisplay;
        u8g2_Setup_ssd1306_128x64_noname_f(
            transitionDisplay.getU8g2(), U8G2_R0,
            u8x8_byte_empty, u8x8_dummy_cb);
        PixelUI transitionUi(transitionDisplay);
        ViewManager& transitionManager = *transitionUi.getViewManagerPtr();
        const AppItem transitioning =
            AppItem::make<EnterTransitionApplication>("Transition", nullptr);

        if (transitionManager.launch(transitioning, nullptr) !=
            ViewManager::LaunchResult::Ok) return 16;
        if (transitionUi.activeAnimationCount() != 1U ||
            !transitionUi.renderer()) return 17;
        if (!pixelIsSet(transitionDisplay, 10, 5) ||
            pixelIsSet(transitionDisplay, 5, 5) ||
            !pixelIsSet(transitionDisplay, 4, 5) ||
            transitionUi.getCanvas().camera().x() != -5) return 18;

        transitionUi.Heartbeat(150U);
        if (!transitionUi.renderer() ||
            !pixelIsSet(transitionDisplay, 5, 5) ||
            pixelIsSet(transitionDisplay, 10, 5) ||
            !pixelIsSet(transitionDisplay, 4, 5) ||
            transitionUi.activeAnimationCount() != 0U) return 19;
    }

    return 0;
}
