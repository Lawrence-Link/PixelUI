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

#include <functional>
#include <stdint.h>
#include <etl/vector.h>
#include "IApplication.h"
#include "PixelUI/config.h"
#include <algorithm>

enum class MenuItemType {
    Action,
    App,
}; 

struct AppItem {
    const char* title;
    const uint8_t* bitmap;
    // Factory function to create an instance of the application
    std::function<std::shared_ptr<IApplication>(PixelUI&)> createApp;
    MenuItemType type;
    int8_t order = -1; 
};

class AppManager{
public:
    // singleton pattern
    static AppManager& getInstance() {
        static AppManager instance;
        return instance;
    }
    ~AppManager() = default;

    void registerApp(const AppItem& item);
    void sortByOrder();
    const etl::vector<AppItem, MAX_APP_NUM>& getAppVector() const;

    AppManager(const AppManager&) = delete; // disable copy constructor
    AppManager& operator=(const AppManager&) = delete; // disable assignment operator

private:
    AppManager() {};
    etl::vector<AppItem, MAX_APP_NUM> appItems_;
};

class AppRegistrar {
public:
    AppRegistrar(const AppItem& item) {
        AppManager::getInstance().registerApp(item);
    }
};