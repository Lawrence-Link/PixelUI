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

#include "core/app/IApplication.h"
#include "core/app/app_system.h"
#include "core/coroutine/Coroutine.h"

static const unsigned char image_greenhouse_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0xff,0x00,0xff,0x7f,0x3e,0xff,0x7f,0x3f,0xff,0xff,0x3f,0xff,0xff,0x1f,0xff,0xff,0x8f,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};

class Greenhouse_App : public IApplication {

private:
    PixelUI &m_ui;
    Coroutine anim_coroutine;
public:
    Greenhouse_App(PixelUI& ui) :
    m_ui(ui),
    anim_coroutine([this](CoroutineContext& ctx) {
        CORO_BEGIN(ctx);
        
        CORO_DELAY(ctx, m_ui, 160, 100);

        CORO_DELAY(ctx, m_ui, 200, 200);

        CORO_END(ctx);}) 
    {}

    void onEnter(ExitCallback exitCallback) override {}
    void onExit() override {}
    bool handleInput(InputEvent event) override {}

    void draw() override 
    {
        U8G2& u8g2 = m_ui.getU8G2();

    }
};

#if USE_STATIC_APP_REGISTER_ENABLED
static AppRegistrar registrar_greenhouse_app({
    .title = "Greenhouse",
    .bitmap = image_greenhouse_bits, // TODO: Add an icon bitmap here

    // Provide a factory function to create application instance.
    .createApp = [](PixelUI& ui) -> std::shared_ptr<IApplication> {
        return std::make_shared<Greenhouse_App>(ui);
    },

    .order = 2
});
#else

AppItem greenhouse_app{
    .title = "Greenhouse",
    .bitmap = image_greenhouse_bits, // TODO: Add an icon bitmap here

    // Provide a factory function to create application instance.
    .createApp = [](PixelUI& ui) -> std::shared_ptr<IApplication> {
        return std::make_shared<Greenhouse_App>(ui);
    },
};

#endif
