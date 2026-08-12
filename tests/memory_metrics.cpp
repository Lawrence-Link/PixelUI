#include "core/ViewManager/ViewManager.h"
#include "core/animation/animation.h"
#include "ui/IconView/IconView.h"
#include "ui/ListView/ListView.h"

#include <stdio.h>

int main() {
    printf("sizeof(ApplicationStack)=%zu\n", sizeof(ApplicationStack));
    printf("alignof(ApplicationStack)=%zu\n", alignof(ApplicationStack));
    printf("sizeof(ViewManager)=%zu\n", sizeof(ViewManager));
    printf("sizeof(IApplication)=%zu alignof(IApplication)=%zu\n",
           sizeof(IApplication), alignof(IApplication));
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
    printf("arena=%zu metadata_and_padding=%zu\n",
           APPLICATION_ARENA_SIZE,
           sizeof(ApplicationStack) - APPLICATION_ARENA_SIZE);
    return 0;
}
