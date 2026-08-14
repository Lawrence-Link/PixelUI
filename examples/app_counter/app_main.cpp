// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2025, Lawrence Link

#include "app_counter.h"

#include "app_resources.h"
#include "core/app/app_system.h"

AppItem counter_app = AppItem::make<APP_COUNTER>(
    "COUNTER",
    app_counter_resources::counter_icon);
