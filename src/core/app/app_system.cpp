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

#include "core/app/app_system.h"

/*
* @brief Registers a new application with the AppManager.
* @param item The AppItem structure containing application details.
*/
void AppManager::registerApp(const AppItem& item) {
    appItems_.push_back(item);
}

/*
* @brief Retrieves the vector of registered applications.
* @return A constant reference to the vector of AppItem structures.
*/
const etl::vector<AppItem, MAX_APP_NUM>& AppManager::getAppVector() const {
    return appItems_;
}
/*
@brief Sorts the registered applications based on their order and title.
*/
void AppManager::sortByOrder() {
    std::sort(appItems_.begin(), appItems_.end(), [](const AppItem& a, const AppItem& b) {
            // sort by order if both are valid
            if (a.order >= 0 && b.order >= 0) {
                return a.order < b.order;
            }
            // if only one is valid, it goes first
            if (a.order >= 0) return true;
            if (b.order >= 0) return false;

            // if both are invalid, sort by title alphabetically
            return strcmp(a.title, b.title) < 0;
        });
}