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

#include "PixelUI.h"
#include "core/app/app_system.h"
#include "ui/ListView/ListView.h"

static const unsigned char image_LISTVIEW_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x7c,0xe3,0xff,0xff,0xf7,0x07,0x7f,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x78,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x7c,0xe3,0xff,0xff,0xff,0xdf,0x45,0xfc,0xdf,0xe5,0xfe,0x1e,0xcd,0x7e,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

bool bool_state = false;

extern PixelUI ui;

int my_value = 0;

ListItem sub_CathyFlower[3] = {
    ListItem(">>> Sub Menu <<<", nullptr, 0, [](){  }),
    ListItem("- Progress", nullptr, 0, [](){  }),
    ListItem("- Alert", nullptr, 0, [](){  })
};

ListItem ItemList[10] = {
    ListItem(">>> ListDemo <<<"),
    ListItem("- Show pop", nullptr, 0, [](){ ui.showPopupInfo("Hello from PixelUI!", "Info", 80, 30, 2000); }),
    ListItem("- Sub Menu", sub_CathyFlower, 3),
    ListItem("- Bool State", nullptr, 0, nullptr, {.switchValue = &bool_state}),
    ListItem("- Value", nullptr, 0, [](){ ui.showPopupProgress(my_value, 0, 100, "Value", 100, 40, 5000, 1); }, {.intValue = &my_value}),
    ListItem("- Alert", nullptr, 0, [](){  }),
    ListItem("- Progress", nullptr, 0, [](){  }),
    ListItem("- Anytone", nullptr, 0, [](){  }),
    ListItem("- Potato", nullptr, 0, [](){  }),
    ListItem("- Tomato", nullptr, 0, [](){  })
};

class ListViewDemo : public ListView {
public:
    ListViewDemo(PixelUI& ui, ListItem *itemList, size_t length) : ListView(ui, itemList, length) {}
};

static AppRegistrar registrar_about_app({
    .title = "ListView Test",
    .bitmap = image_LISTVIEW_bits,
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> {
        return std::make_unique<ListView>(ui, ItemList, 10);
    },
    .type = MenuItemType::App,
    .order = 6
});