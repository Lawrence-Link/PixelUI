#include "PixelUI/core/app/app_system.h"

void AppManager::registerApp(const AppItem& item) {
    appItems_.push_back(item);
    std::cout << "Registered app: " << item.title << std::endl;
}

const std::vector<AppItem>& AppManager::getAppVector() const {
    return appItems_;
}