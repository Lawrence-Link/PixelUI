#include "core/ViewManager/ViewManager.h"
#include "core/animation/animation.h"
#include "ui/IconView/IconView.h"
#include "ui/ListView/ListView.h"
#include "ui/Popup/PopupInfo.h"
#include "ui/Popup/PopupManager.h"
#include "ui/Popup/PopupProgress.h"
#include "ui/Popup/PopupValueDigits.h"
#include "focus/focus.h"
#include "blink/BlinkState.h"
#include "core/scheduling/UiDeadlineScheduler.h"

#include <stdio.h>

namespace {

enum class LegacyRequestType : uint8_t {
    None,
    Info,
    Progress,
    ValueDigits,
};

struct LegacyPopupRequestLayout {
    LegacyRequestType type = LegacyRequestType::None;
    uint16_t width = 0;
    uint16_t height = 0;
    uint16_t duration = 0;
    const char* text = nullptr;
    const char* title = nullptr;
    int32_t* value = nullptr;
    int32_t minValue = 0;
    int32_t maxValue = 0;
    const uint8_t* font = nullptr;
    ValueCallback callback;
    bool legacyProgressDisplayFlag = false;
    uint8_t digitCount = 0;
};

} // namespace

int main() {
    printf("sizeof(ApplicationStack)=%zu\n", sizeof(ApplicationStack));
    printf("alignof(ApplicationStack)=%zu\n", alignof(ApplicationStack));
    printf("sizeof(ViewManager)=%zu\n", sizeof(ViewManager));
    printf("sizeof(IApplication)=%zu alignof(IApplication)=%zu\n",
           sizeof(IApplication), alignof(IApplication));
    printf("sizeof(IWidget)=%zu alignof(IWidget)=%zu\n",
           sizeof(IWidget), alignof(IWidget));
    printf("sizeof(FocusManager)=%zu alignof(FocusManager)=%zu\n",
           sizeof(FocusManager), alignof(FocusManager));
    printf("sizeof(IconView)=%zu alignof(IconView)=%zu\n",
           sizeof(IconView), alignof(IconView));
    printf("sizeof(ListView)=%zu alignof(ListView)=%zu\n",
           sizeof(ListView), alignof(ListView));
    printf("sizeof(Animation)=%zu alignof(Animation)=%zu\n",
           sizeof(Animation), alignof(Animation));
    printf("sizeof(CallbackAnimation)=%zu alignof(CallbackAnimation)=%zu\n",
           sizeof(CallbackAnimation), alignof(CallbackAnimation));
    printf("sizeof(AnimationManager)=%zu alignof(AnimationManager)=%zu\n",
           sizeof(AnimationManager), alignof(AnimationManager));
    printf("sizeof(PixelUI)=%zu alignof(PixelUI)=%zu\n",
           sizeof(PixelUI), alignof(PixelUI));
    printf("sizeof(UiDeadlineScheduler)=%zu alignof(UiDeadlineScheduler)=%zu\n",
           sizeof(UiDeadlineScheduler), alignof(UiDeadlineScheduler));
    printf("sizeof(BlinkState)=%zu alignof(BlinkState)=%zu\n",
           sizeof(BlinkState), alignof(BlinkState));
    printf("sizeof(PopupInfo)=%zu alignof(PopupInfo)=%zu\n",
           sizeof(PopupInfo), alignof(PopupInfo));
    printf("sizeof(PopupProgress)=%zu alignof(PopupProgress)=%zu\n",
           sizeof(PopupProgress), alignof(PopupProgress));
    printf("sizeof(PopupValueDigits)=%zu alignof(PopupValueDigits)=%zu\n",
           sizeof(PopupValueDigits), alignof(PopupValueDigits));
    printf("sizeof(PopupManager)=%zu alignof(PopupManager)=%zu\n",
           sizeof(PopupManager), alignof(PopupManager));
    printf("sizeof(LegacyPopupRequestLayout)=%zu\n",
           sizeof(LegacyPopupRequestLayout));
    printf("sizeof(RequestEnvelope)=%zu sizeof(InfoRequest)=%zu\n",
           sizeof(RequestEnvelope), sizeof(InfoRequest));
    printf("sizeof(ProgressRequest)=%zu sizeof(ValueDigitsRequest)=%zu\n",
           sizeof(ProgressRequest), sizeof(ValueDigitsRequest));
    printf("sizeof(PopupRequest)=%zu\n", sizeof(PopupRequest));
    printf("arena=%zu metadata_and_padding=%zu\n",
           APPLICATION_ARENA_SIZE,
           sizeof(ApplicationStack) - APPLICATION_ARENA_SIZE);
    return 0;
}
