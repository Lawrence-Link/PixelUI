#pragma once

#include "PixelUI.h"
#include "core/ViewManager/ViewManager.h"

class AppLauncher {
public:
    static ViewManager::LaunchResult launch(PixelUI& ui, ViewManager& viewManager);
};
