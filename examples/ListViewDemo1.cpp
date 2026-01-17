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

// --- USER DEFINED APP: A ListView Demo ---

#include "PixelUI.h"
#include "core/app/app_system.h"
#include "ui/ListView/ListView.h"

static const unsigned char image_LISTVIEW_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x7c,0xe3,0xff,0xff,0xf7,0x07,0x7f,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x78,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x7c,0xe3,0xff,0xff,0xff,0xdf,0x45,0xfc,0xdf,0xe5,0xfe,0x1e,0xcd,0x7e,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

bool bool_state = false;

extern PixelUI ui;

int my_value = 0;
int my_value_4_digits = 0;

ListItem sub_CathyFlower[3] = {
    ListItem(">>> Sub Menu <<<"),
    ListItem("- Text"),
    ListItem("- 进度")
};

ListItem ItemList[10] = {
    ListItem(">>> ListDemo <<<"),
    ListItem{.title ="- Show pop", .pFunc = [](){ ui.showPopupInfo("Hello from PixelUI!", "Info", 80, 30, 2000); }},
    ListItem{.title ="- Sub Menu", .nextList = sub_CathyFlower, .nextListLength = 3},
    ListItem{.title ="- Bool State", .extra = {.switchValue = &bool_state}},
    ListItem{.title ="- Bool Value", .pFunc = [](){ ui.showPopupProgress(my_value, 0, 100, "Value", 100, 40, 5000, 1); }, .extra = {.intValue = &my_value}},
    ListItem{.title ="- Show 4 Digits", .pFunc = [](){ ui.showPopupValue4Digits(my_value_4_digits, "Value 4D", 100, 40, 5000, 1); }},
    ListItem{.title ="- Progress"},
    ListItem{.title ="- Anytone"},
    ListItem{.title ="- Potato"},
    ListItem{.title ="- Tomato"}
};

class ListViewDemo : public ListView {
public:
    ListViewDemo(PixelUI& ui, ListItem *itemList, size_t length) : ListView(ui, itemList, length) {}

    void onLoad() override {  } // will be called when loading ListViewDemo
    void onSave() override {  } // will be called when requesting exit
};

// ---------------- Application registration ----------------
#if USE_STATIC_APP_REGISTER_ENABLED
static AppRegistrar registrar_ListViewDemo_app({
    .title = "ListView Test",
    .bitmap = image_LISTVIEW_bits,
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> {
        return std::make_unique<ListViewDemo>(ui, ItemList, 10);
    },
    .order = 6
});
#else // register manually
    AppItem ListViewDemo_app{
    .title = "ListView Test",
    .bitmap = image_LISTVIEW_bits,
    .createApp = [](PixelUI& ui, void* parameter) -> std::unique_ptr<IApplication> {
        return std::make_unique<ListViewDemo>(ui, ItemList, 10);
    },
};
#endif