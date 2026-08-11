#pragma once

#include "PixelUI.h"
#include "core/ViewManager/ViewManager.h"

class AppLauncher {
public:
    // Factory method: creates and returns a fully configured app launcher view.
    static ApplicationPtr createAppLauncherView(PixelUI& ui, ViewManager& viewManager);
};
