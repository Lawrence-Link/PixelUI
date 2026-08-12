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

AppItem ListViewDemo_app = AppItem::make<ListViewDemo>(
    "ListView Test",
    image_LISTVIEW_bits,
    [](void* storage, PixelUI& ui, void*) -> IApplication* {
        return ::new (storage) ListViewDemo(ui, ItemList, 10);
    });
