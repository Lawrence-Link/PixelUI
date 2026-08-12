#include "PixelUI.h"
#include "core/animation/animation.h"

namespace {

ValueCallback noOpCallback() {
    return [](int32_t) {};
}

} // namespace

int main() {
    {
        AnimationManager manager;
        int32_t value = 0;
        if (!manager.emplace(
                0,
                100,
                100,
                EasingType::LINEAR,
                [&value](int32_t current) { value = current; },
                PROTECTION::NOT_PROTECTED,
                0)) return 1;
        if ((manager.activeCount() != 1U) || (manager.available() != MAX_ANIMATION_COUNT - 1U)) return 2;

        manager.update(50);
        if ((value != 50) || (manager.activeCount() != 1U)) return 3;
        manager.update(100);
        if ((value != 100) || (manager.activeCount() != 0U)) return 4;
    }

    {
        AnimationManager manager;
        if (!manager.emplace(
                0, 1, 100, EasingType::LINEAR, noOpCallback(),
                PROTECTION::PROTECTED, 0)) return 5;
        if (!manager.emplace(
                0, 1, 100, EasingType::LINEAR, noOpCallback(),
                PROTECTION::NOT_PROTECTED, 0)) return 6;

        manager.clearUnprotected();
        if (manager.activeCount() != 1U) return 7;
        manager.clearAllProtectionMarks();
        manager.clearUnprotected();
        if (manager.activeCount() != 0U) return 8;
    }

    {
        AnimationManager manager;
        for (size_t i = 0; i < MAX_ANIMATION_COUNT; ++i) {
            if (!manager.emplace(
                    0, 1, 100, EasingType::LINEAR, noOpCallback(),
                    PROTECTION::NOT_PROTECTED, 0)) return 9;
        }
        if (manager.emplace(
                0, 1, 100, EasingType::LINEAR, noOpCallback(),
                PROTECTION::NOT_PROTECTED, 0)) return 10;
        if ((manager.activeCount() != MAX_ANIMATION_COUNT) || (manager.available() != 0U)) return 11;
    }

    {
        U8G2 display;
        PixelUI ui(display);
        for (size_t i = 0; i < MAX_ANIMATION_COUNT - 1U; ++i) {
            if (!ui.animateCallback(
                    0, 1, 100, EasingType::LINEAR, noOpCallback())) return 12;
        }

        int32_t x = 1;
        int32_t y = 2;
        if (ui.animate(x, y, 10, 20, 100)) return 13;
        if ((ui.activeAnimationCount() != MAX_ANIMATION_COUNT - 1U) ||
            (x != 1) || (y != 2)) return 13;
    }

    return 0;
}
