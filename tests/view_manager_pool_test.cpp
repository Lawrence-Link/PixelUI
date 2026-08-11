#include "PixelUI.h"
#include "core/ViewManager/ViewManager.h"

struct LifecycleState {
    int entered = 0;
    int paused = 0;
    int resumed = 0;
    int exited = 0;
    int destroyed = 0;
};

class LifecycleApplication : public IApplication {
public:
    LifecycleApplication(PixelUI&, void* parameters)
        : state_(*static_cast<LifecycleState*>(parameters)) {
    }

    ~LifecycleApplication() override {
        ++state_.destroyed;
    }

    void draw() override {
    }

    bool handleInput(InputEvent) override {
        return false;
    }

    void onEnter(ExitCallback exitCallback) override {
        IApplication::onEnter(exitCallback);
        ++state_.entered;
    }

    void onPause() override {
        ++state_.paused;
    }

    void onResume() override {
        ++state_.resumed;
    }

    void onExit() override {
        ++state_.exited;
    }

private:
    LifecycleState& state_;
};

int main() {
    U8G2 display;
    PixelUI ui(display);
    ViewManager& manager = *ui.getViewManagerPtr();
    LifecycleState state;
    AppItem item = AppItem::make<LifecycleApplication>("Test", nullptr);

    if (manager.launch(item, &state) != ViewManager::LaunchResult::Ok) {
        return 1;
    }
    if (manager.launch(item, &state) != ViewManager::LaunchResult::Ok) {
        return 2;
    }
    if ((state.entered != 2) || (state.paused != 1)) {
        return 3;
    }

    manager.pop();
    if ((state.exited != 1) || (state.destroyed != 1) ||
        (state.resumed != 1)) {
        return 4;
    }

    for (size_t i = 1; i < APPLICATION_POOL_CAPACITY; ++i) {
        if (manager.launch(item, &state) != ViewManager::LaunchResult::Ok) {
            return 5;
        }
    }
    if (manager.launch(item, &state) != ViewManager::LaunchResult::StackFull) {
        return 6;
    }

    for (size_t i = 0; i < APPLICATION_POOL_CAPACITY; ++i) {
        manager.pop();
    }
    if ((manager.getCurrentApp() != nullptr) ||
        (state.destroyed != APPLICATION_POOL_CAPACITY + 1)) {
        return 7;
    }

    AppItem invalidItem{"Invalid", nullptr, nullptr};
    if (manager.launch(invalidItem) != ViewManager::LaunchResult::ConstructionFailed) {
        return 8;
    }

    {
        ApplicationPool foreignPool;
        ApplicationPtr foreign = foreignPool.make<LifecycleApplication>(ui, &state);
        ViewManager::LaunchResult result = manager.push(etl::move(foreign));
        if (result != ViewManager::LaunchResult::ConstructionFailed) {
            return 9;
        }
        if (!foreignPool.empty()) {
            return 10;
        }
    }

    {
        ApplicationPtr held[APPLICATION_POOL_CAPACITY];
        for (size_t i = 0; i < APPLICATION_POOL_CAPACITY; ++i) {
            held[i] = manager.makeApplication<LifecycleApplication>(ui, &state);
            if (!held[i]) {
                return 11;
            }
        }
        if (manager.launch(item, &state) != ViewManager::LaunchResult::PoolFull) {
            return 12;
        }
    }

    return (state.destroyed == (2 * APPLICATION_POOL_CAPACITY) + 2) ? 0 : 13;
}
