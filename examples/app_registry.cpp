/*
 * Copyright (C) 2025 Lawrence Link
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "app_registry.h"

#if (USE_STATIC_APP_REGISTER_ENABLED == 0)

void registerApps() {
    auto& app_man = AppManager::getInstance(); // acquire for global AppManager singleton..
    
    app_man.registerApp(counter_app);
    app_man.registerApp(cube_demo_app);
    app_man.registerApp(bouncy_about_app);
    app_man.registerApp(charging_anim);
    app_man.registerApp(ListViewDemo_app);
    app_man.registerApp(time_setting_app);
}

#endif