#include "core/ViewManager/ViewManager.h"
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
    printf("arena=%zu metadata_and_padding=%zu\n",
           APPLICATION_ARENA_SIZE,
           sizeof(ApplicationStack) - APPLICATION_ARENA_SIZE);
    return 0;
}
