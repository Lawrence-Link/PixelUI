#include "PixelUI.h"
#include "core/ViewManager/ViewManager.h"
#include "ui/IconView/IconView.h"
#include "widgets/IWidget.h"

#include <string.h>

namespace {

struct EventLog {
    char events[128]{};
    size_t size = 0;

    void add(char event) { events[size++] = event; }
    bool equals(const char* expected) const {
        return (strlen(expected) == size) && (memcmp(events, expected, size) == 0);
    }
};

struct LifecycleState {
    EventLog* log = nullptr;
    int id = 0;
};

class LifecycleApplication : public IApplication {
public:
    LifecycleApplication(PixelUI&, void* parameters)
        : state_(*static_cast<LifecycleState*>(parameters)) {}

    ~LifecycleApplication() override { state_.log->add(static_cast<char>('0' + state_.id)); }
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }
    void onEnter(ExitCallback callback) override {
        IApplication::onEnter(callback);
        state_.log->add('E');
    }
    void onPause() override { state_.log->add('P'); }
    void onResume() override { state_.log->add('R'); }
    void onExit() override { state_.log->add('X'); }

private:
    LifecycleState& state_;
};

class TestWidget : public IWidget {
public:
    TestWidget() { setFocusable(true); }
    void onLoad() override {}
    void onOffload() override {}

private:
    void drawSelf(const WidgetRenderContext&) override {}
    Canvas& display() override { return *display_; }

public:
    void attach(Canvas& display) { display_ = &display; }

private:
    Canvas* display_ = nullptr;
};

class ReferencingApplication : public IApplication {
public:
    ReferencingApplication(PixelUI& ui, void* parameters)
        : ui_(ui), cleanAtDestruction_(*static_cast<bool*>(parameters)),
          coroutine_([](CoroutineContext&) {}) {}

    ~ReferencingApplication() override {
        cleanAtDestruction_ =
            (ui_.getActiveCoroutineCount() == 0U) &&
            (ui_.activeAnimationCount() == 0U) &&
            (ui_.popupCount() == 0U) &&
            (ui_.getFocusedWidgetCount() == 0U);
    }

    void draw() override {}
    bool handleInput(InputEvent) override { return false; }
    void onEnter(ExitCallback callback) override {
        IApplication::onEnter(callback);
        widget_.attach(ui_.getCanvas());
        ui_.addWidgetToFocusManager(&widget_);
        ui_.addCoroutine(&coroutine_);
        ui_.animate(value_, 10, 100);
        ui_.showPopupProgress(value_, 0, 100, "Value", 80, 30, 3000);
    }

private:
    PixelUI& ui_;
    bool& cleanAtDestruction_;
    int32_t value_ = 0;
    Coroutine coroutine_;
    TestWidget widget_;
};

struct ReentrantState {
    ViewManager* manager = nullptr;
    const AppItem* item = nullptr;
    int rejected = 0;
    int destroyed = 0;
    bool enterPop = false;
    bool pauseLaunch = false;
    bool exitPop = false;
    bool resumeLaunch = false;
    bool destructorPop = false;
};

class ReentrantApplication : public IApplication {
public:
    ReentrantApplication(PixelUI&, void* parameters)
        : state_(*static_cast<ReentrantState*>(parameters)) {}

    ~ReentrantApplication() override {
        ++state_.destroyed;
        if (state_.destructorPop && !state_.manager->pop()) ++state_.rejected;
    }
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }
    void onEnter(ExitCallback callback) override {
        IApplication::onEnter(callback);
        if (state_.enterPop && !state_.manager->pop()) ++state_.rejected;
    }
    void onPause() override {
        if (state_.pauseLaunch &&
            state_.manager->launch(*state_.item, &state_) == ViewManager::LaunchResult::TransitionInProgress) {
            ++state_.rejected;
        }
    }
    void onExit() override {
        if (state_.exitPop && !state_.manager->pop()) ++state_.rejected;
    }
    void onResume() override {
        if (state_.resumeLaunch &&
            state_.manager->launch(*state_.item, &state_) == ViewManager::LaunchResult::TransitionInProgress) {
            ++state_.rejected;
        }
    }

private:
    ReentrantState& state_;
};

class ArenaConsumer : public IApplication {
public:
    explicit ArenaConsumer(PixelUI&) {}
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }

private:
    uint8_t payload_[APPLICATION_ARENA_SIZE - 64U]{};
};

class FadeOnPauseApplication : public LifecycleApplication {
public:
    FadeOnPauseApplication(PixelUI& ui, void* parameters)
        : LifecycleApplication(ui, parameters), ui_(ui) {}

    void onPause() override {
        LifecycleApplication::onPause();
        ui_.markFading();
    }

private:
    PixelUI& ui_;
};

IApplication* failConstruction(void*, PixelUI&, void*) {
    return nullptr;
}

} // namespace

int main() {
    {
        U8G2 display;
        PixelUI ui(display);
        IconView iconView(ui);
        IconItemList items;
        items.emplace_back("Item", nullptr, nullptr);

        iconView.setItems(items);
        iconView.setSelectionCallback([](int, const IconItem&) {});
        if ((ui.activeAnimationCount() != 0U) ||
            (ui.getActiveCoroutineCount() != 0U) ||
            (ui.getFocusedWidgetCount() != 0U) ||
            (ui.popupCount() != 0U)) return 1;

        iconView.onEnter(nullptr);
        if (ui.activeAnimationCount() == 0U) return 2;
        ui.clearAllAnimations();
    }

    {
        U8G2 display;
        PixelUI ui(display);
        ViewManager& manager = *ui.getViewManagerPtr();
        EventLog log;
        LifecycleState first{&log, 1};
        LifecycleState second{&log, 2};
        AppItem item = AppItem::make<LifecycleApplication>("Lifecycle", nullptr);

        if (manager.launch(item, &first) != ViewManager::LaunchResult::Ok) return 3;
        if (manager.launch(item, &second) != ViewManager::LaunchResult::Ok) return 4;
        if (!log.equals("EPE")) return 5;
        if (!manager.pop() || !log.equals("EPEX2R")) return 6;
        if (!manager.pop() || !log.equals("EPEX2RX1")) return 7;
        if (manager.pop() || (manager.getCurrentApp() != nullptr) || (manager.getViewDepth() != 0U)) return 8;
    }

    {
        EventLog log;
        LifecycleState first{&log, 1};
        LifecycleState second{&log, 2};
        U8G2 display;
        u8g2_Setup_ssd1306_128x64_noname_f(
            display.getU8g2(), U8G2_R0, u8x8_byte_empty, u8x8_dummy_cb);
        PixelUI ui(display);
        ViewManager& manager = *ui.getViewManagerPtr();
        AppItem fading = AppItem::make<FadeOnPauseApplication>("Fade", nullptr);
        AppItem next = AppItem::make<LifecycleApplication>("Next", nullptr);

        if (manager.launch(fading, &first) != ViewManager::LaunchResult::Ok) return 30;
        if (manager.launch(next, &second) != ViewManager::LaunchResult::Ok) return 31;
        if (!log.equals("EP") || !manager.isTransitioning()) return 32;
        if (manager.launch(next, &second) != ViewManager::LaunchResult::TransitionInProgress) return 33;

        for (int step = 0; step < 3; ++step) {
            ui.Heartbeat(40);
            ui.renderer();
            if (!log.equals("EP")) return 34;
        }
        ui.Heartbeat(40);
        ui.renderer();
        if (!log.equals("EPE") || manager.isTransitioning()) return 35;
    }

    {
        U8G2 display;
        PixelUI ui(display);
        ViewManager& manager = *ui.getViewManagerPtr();
        AppItem referencing = AppItem::make<ReferencingApplication>("References", nullptr);
        bool cleanAtDestruction = false;
        if (manager.launch(referencing, &cleanAtDestruction) != ViewManager::LaunchResult::Ok) return 7;
        if ((ui.getActiveCoroutineCount() != 1U) ||
            (ui.activeAnimationCount() == 0U) ||
            (ui.popupCount() != 1U) ||
            (ui.getFocusedWidgetCount() != 1U)) return 8;
        if (!manager.pop()) return 9;
        if ((ui.getActiveCoroutineCount() != 0U) ||
            (ui.activeAnimationCount() != 0U) ||
            (ui.popupCount() != 0U) ||
            (ui.getFocusedWidgetCount() != 0U) || !cleanAtDestruction) return 10;
    }

    {
        U8G2 display;
        PixelUI ui(display);
        ViewManager& manager = *ui.getViewManagerPtr();
        ReentrantState state;
        AppItem item = AppItem::make<ReentrantApplication>("Reentrant", nullptr);
        state.manager = &manager;
        state.item = &item;
        state.enterPop = true;
        state.pauseLaunch = true;
        state.exitPop = true;
        state.resumeLaunch = true;
        state.destructorPop = true;

        if (manager.launch(item, &state) != ViewManager::LaunchResult::Ok) return 11;
        if (manager.launch(item, &state) != ViewManager::LaunchResult::Ok) return 12;
        if (!manager.pop()) return 13;
        if (!manager.pop()) return 14;
        if ((state.rejected != 8) || (state.destroyed != 2) || (manager.getViewDepth() != 0U)) return 15;
    }

    {
        U8G2 display;
        PixelUI ui(display);
        ViewManager& manager = *ui.getViewManagerPtr();
        EventLog log;
        LifecycleState state{&log, 1};
        AppItem item = AppItem::make<LifecycleApplication>("Small", nullptr);
        for (size_t i = 0; i < MAX_VIEW_DEPTH; ++i) {
            if (manager.launch(item, &state) != ViewManager::LaunchResult::Ok) return 16;
        }
        const size_t used = manager.getArenaUsed();
        if (manager.launch(item, &state) != ViewManager::LaunchResult::StackFull) return 17;
        if ((manager.getArenaUsed() != used) || (manager.getViewDepth() != MAX_VIEW_DEPTH)) return 18;
        while (manager.pop()) {}
    }

    {
        U8G2 display;
        PixelUI ui(display);
        ViewManager& manager = *ui.getViewManagerPtr();
        EventLog log;
        LifecycleState state{&log, 1};
        AppItem item = AppItem::make<LifecycleApplication>("Small", nullptr);
        if (manager.launch(item, &state) != ViewManager::LaunchResult::Ok) return 19;
        const size_t used = manager.getArenaUsed();
        if (manager.push<ArenaConsumer>(ui) != ViewManager::LaunchResult::ArenaFull) return 20;
        if ((manager.getArenaUsed() != used) || (manager.getViewDepth() != 1U)) return 21;

        AppItem invalid{"Invalid", nullptr, ApplicationFactory{}};
        if (manager.launch(invalid) != ViewManager::LaunchResult::ConstructionFailed) return 22;
        AppItem failing = AppItem::make<LifecycleApplication>("Fail", nullptr, &failConstruction);
        if (manager.launch(failing, &state) != ViewManager::LaunchResult::ConstructionFailed) return 23;
        if ((manager.getArenaUsed() != used) || (manager.getViewDepth() != 1U)) return 24;
        if (!manager.pop()) return 25;
    }

    EventLog destructionLog;
    LifecycleState one{&destructionLog, 1};
    LifecycleState two{&destructionLog, 2};
    LifecycleState three{&destructionLog, 3};
    {
        U8G2 display;
        PixelUI ui(display);
        ViewManager& manager = *ui.getViewManagerPtr();
        AppItem item = AppItem::make<LifecycleApplication>("Destroy", nullptr);
        if (manager.launch(item, &one) != ViewManager::LaunchResult::Ok) return 26;
        if (manager.launch(item, &two) != ViewManager::LaunchResult::Ok) return 27;
        if (manager.launch(item, &three) != ViewManager::LaunchResult::Ok) return 28;
    }
    if (!destructionLog.equals("EPEPE321")) return 29;

    return 0;
}
