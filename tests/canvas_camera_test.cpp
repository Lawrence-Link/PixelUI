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
    if (!pixelIsSet(display, 4, 5) || pixelIsSet(display, 3, 63)) return 5;

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

    return 0;
}
