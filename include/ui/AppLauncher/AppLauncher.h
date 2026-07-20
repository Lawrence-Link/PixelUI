#pragma once

#include "PixelUI.h"
#include "core/app/IApplication.h"
#include "core/ViewManager/ViewManager.h"
#include <etl/memory.h>

class AppLauncher {
public:
    // Factory method: creates and returns a fully configured app launcher view.
    static etl::unique_ptr<IApplication> createAppLauncherView(PixelUI& ui, ViewManager& viewManager);
};
