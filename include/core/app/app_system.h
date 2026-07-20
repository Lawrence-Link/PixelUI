/*
* Copyright (C) 2025 Lawrence Link
*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <stdint.h>
#include <etl/vector.h>
#include <etl/inplace_function.h>
#include <etl/memory.h>
#include "IApplication.h"
#include "config.h"

enum class MenuItemType {
    Action,
    App,
};

struct AppItem {
    /* Name of the app */
    const char* title;
    /* Bitmap to the app icon */
    const uint8_t* bitmap;
    /* Factory function of the app */
    etl::inplace_function<etl::unique_ptr<IApplication>(PixelUI&, void* parameters), CALLBACK_STORAGE_SIZE> createApp;
};

class AppManager {
public:
    static AppManager& getInstance() {
        static AppManager instance;
        return instance;
    }
    void registerApp(const AppItem& item);
    const etl::vector<AppItem, MAX_APP_NUM>& getAppVector() const;
    size_t getRegisteredCount() const { return appItems_.size(); }
    AppManager(const AppManager&) = delete;
    AppManager& operator=(const AppManager&) = delete;
private:
    AppManager() = default;
    etl::vector<AppItem, MAX_APP_NUM> appItems_;
};
