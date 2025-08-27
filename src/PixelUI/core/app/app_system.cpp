#include "PixelUI/core/app/app_system.h"

void AppManager::registerApp(const AppItem& item) {
    appItems_.push_back(item);
    #ifdef DEBUG
    std::cout << "Registered app: " << item.title << std::endl;
    #endif
}

const etl::vector<AppItem, MAX_APP_NUM>& AppManager::getAppVector() const {
    return appItems_;
}