
#include "app_registry.h"

#if (USE_STATIC_APP_REGISTER_ENABLED == 0)

void registerApps() {
    auto& app_man = AppManager::getInstance(); // acquire for global AppManager singleton..

    app_man.registerApp(counter_app);
    app_man.registerApp(cube_demo_app);
    app_man.registerApp(bouncy_about_app);
    app_man.registerApp(charging_anim);
    app_man.registerApp(ListViewDemo_app);
}

#endif