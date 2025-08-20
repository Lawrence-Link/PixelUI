#pragma once

#include <functional>
#include <stdint.h>
#include <iostream>
#include <vector>
#include "IApplication.h"

enum class MenuItemType {
    Action,
    App,
}; 

struct AppItem {
    const char* title;
    const uint8_t* bitmap;
    // 工厂函数 现在创建App实例
    std::function<std::shared_ptr<IApplication>()> createApp;
    MenuItemType type;
    int w, h = 0;
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

    const std::vector<AppItem>& getAppVector() const;

    AppManager(const AppManager&) = delete; // disable copy constructor
    AppManager& operator=(const AppManager&) = delete; // disable assignment operator

private:
    AppManager() {}; // private constructor
    std::vector<AppItem> appItems_;
};

class AppRegistrar {
public:
    AppRegistrar(const AppItem& item) {
        AppManager::getInstance().registerApp(item);
    }
};

