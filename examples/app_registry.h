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

#pragma once

#if (USE_STATIC_APP_REGISTER_ENABLED == 0)

#include "core/app/app_system.h"

extern AppItem counter_app;
extern AppItem cube_demo_app;
extern AppItem bouncy_about_app;
extern AppItem charging_anim;
extern AppItem ListViewDemo_app;
extern AppItem time_setting_app;
extern AppItem button_demo_app;
extern AppItem anim_demo_app;
void registerApps();

#endif

