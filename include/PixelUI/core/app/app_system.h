#pragma once

#include <functional>
#include <stdint.h>
#include <iostream>
#include <etl/vector.h>
#include "IApplication.h"
#include "PixelUI/config.h"

enum class MenuItemType {
    Action,
    App,
}; 

struct AppItem {
    const char* title;
    const uint8_t* bitmap;
    // 工厂函数 创建App实例
    std::function<std::shared_ptr<IApplication>(PixelUI&)> createApp;
    MenuItemType type;
    int w, h = 0;
    bool useUnifiedEnterAnimation = false;
};

class AppManager{
public:
    // singleton pattern
    static AppManager& getInstance() {
        static AppManager instance;
        return instance;
    }

    ~AppManager() = default;
    
    // register an app to the Manager.
    void registerApp(const AppItem& item);

    const etl::vector<AppItem, MAX_APP_NUM>& getAppVector() const;

    AppManager(const AppManager&) = delete; // disable copy constructor
    AppManager& operator=(const AppManager&) = delete; // disable assignment operator

private:
    AppManager() {}; // private constructor
    etl::vector<AppItem, MAX_APP_NUM> appItems_;
};

class AppRegistrar {
public:
    AppRegistrar(const AppItem& item) {
        AppManager::getInstance().registerApp(item);
    }
};