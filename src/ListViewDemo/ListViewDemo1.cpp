#include "PixelUI/pixelui.h"
#include "PixelUI/core/app/app_system.h"
#include "PixelUI/core/ui/ListView/ListView.h"

static const unsigned char image_LISTVIEW_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x7c,0xe3,0xff,0xff,0xf7,0x07,0x7f,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x78,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x7c,0xe3,0xff,0xff,0xff,0xdf,0x45,0xfc,0xdf,0xe5,0xfe,0x1e,0xcd,0x7e,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

bool bool_state = false;

extern PixelUI ui;

ListItem sub_CathyFlower[3] = {
    ListItem(">>> Sub Menu <<<", nullptr, 0, [](){ std::cout << "Item C\n"; }),
    ListItem("- Progress", nullptr, 0, [](){ std::cout << "Item C\n"; }),
    ListItem("- Alert", nullptr, 0, [](){ std::cout << "Item C\n"; })
};

ListItem ItemList[10] = {
    ListItem(">>> ListDemo <<<"),
    ListItem("- Show pop", nullptr, 0, [](){ ui.showInfoPopup("Seven human souls.", 1000); }),
    ListItem("- Sub Menu", sub_CathyFlower, 3),
    ListItem("- Bool State", nullptr, 0, nullptr, {.switchValue = &bool_state}),
    ListItem("- Switch", nullptr, 0, [](){ std::cout << "Item E\n"; }),
    ListItem("- Alert", nullptr, 0, [](){ std::cout << "Item F\n"; }),
    ListItem("- Progress", nullptr, 0, [](){ std::cout << "Item G\n"; }),
    ListItem("- Anytone", nullptr, 0, [](){ std::cout << "Item H\n"; }),
    ListItem("- Potato", nullptr, 0, [](){ std::cout << "Item I\n"; }),
    ListItem("- Tomato", nullptr, 0, [](){ std::cout << "Item J\n"; })
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
    .w = 24,
    .h = 24
});