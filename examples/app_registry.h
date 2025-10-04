#pragma once

#if (USE_STATIC_APP_REGISTER_ENABLED == 0)

#include "core/app/app_system.h"

extern AppItem counter_app;
extern AppItem cube_demo_app;
extern AppItem bouncy_about_app;
extern AppItem charging_anim;
extern AppItem ListViewDemo_app;
extern AppItem time_setting_app;

void registerApps();

#endif

