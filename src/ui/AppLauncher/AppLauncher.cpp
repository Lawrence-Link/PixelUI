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

#include "ui/AppLauncher/AppLauncher.h"
#include "ui/IconView/IconView.h"
#include "core/app/app_system.h"

etl::unique_ptr<IApplication> AppLauncher::createAppLauncherView(PixelUI& ui, ViewManager& viewManager) {
    // Create an IconView instance.
    etl::unique_ptr<IconView> iconView(new IconView(ui));

    // Configure the appearance and behavior of the IconView.
    iconView->setTitle("< Apps >");
    iconView->enableProgressBar(true);
    iconView->enableStatusText(true);
    iconView->enableSelectedItemTitle(true);

    // Get the app list from AppManager and convert it to IconItems.
    auto& appManager = AppManager::getInstance();
    const auto& apps = appManager.getAppVector();
    IconItemList iconItems;
    for (const auto& app : apps) {
        // We use const_cast because AppItem* will be stored in a void*,
        // but we guarantee it will not be modified.
        iconItems.emplace_back(app.title, app.bitmap, const_cast<AppItem*>(&app));
    }
    iconView->setItems(iconItems);

    // Set the selection callback to launch the selected application.
    iconView->setSelectionCallback(
        [&ui, &viewManager](int index, const IconItem& item) {
            const AppItem* appItem = static_cast<const AppItem*>(item.userData);
            if (appItem && appItem->createApp) {
                auto appInstance = appItem->createApp(ui, nullptr);
                if (appInstance) {
                    viewManager.push(etl::move(appInstance));
                }
            }
        }
    );
    
    // Return the fully configured view.
    return etl::unique_ptr<IApplication>(etl::move(iconView));
}
