#include "PixelUI/pixelui.h"
#include "PixelUI/core/app/app_system.h"
#include "PixelUI/core/ui/ListView/ListView.h"

static const unsigned char image_LISTVIEW_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x7c,0xe3,0xff,0xff,0xf7,0x07,0x7f,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x78,0xf7,0xff,0xff,0xf7,0x07,0x7e,0xf7,0xff,0xff,0xf7,0x07,0x7c,0xe3,0xff,0xff,0xff,0xdf,0x45,0xfc,0xdf,0xe5,0xfe,0x1e,0xcd,0x7e,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

ListItem sub_CathyFlower[3] = {
    { ">>> Cathy Flower <<<", nullptr, NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Progress",           nullptr, NULL , (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Alert",              nullptr, NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
};

ListItem ItemList[] = { // text example
    { ">>> ListDemo <<<", nullptr,         NULL, (void(*)())nullptr },
    { "- Bob then raped", nullptr,         NULL, (void(*)())[](){ std::cout << "Item B\n"; } },
    { "- Cathy Flower",   sub_CathyFlower, 3,    (void(*)())nullptr },
    { "- Drunken",        nullptr,         NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Switch",         nullptr,         NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Alert",          nullptr,         NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Progress",       nullptr,         NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Anytone",        nullptr,         NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Potato",         nullptr,         NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
    { "- Tomato",         nullptr,         NULL, (void(*)())[](){ std::cout << "Item C\n"; } },
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