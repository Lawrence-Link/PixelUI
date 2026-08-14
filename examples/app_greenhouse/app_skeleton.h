#pragma once

#include "core/app/IApplication.h"
#include "PixelUI.h"
#include "core/app/app_system.h"
#include "core/coroutine/Coroutine.h"
#include "widgets/icon_button/icon_button.h"
#include "widgets/brace/brace.h"
#include "widgets/bitmap/bitmap_widget.h"
#include "app_resources.h"

class Greenhouse_App : public IApplication {
private:
    PixelUI &m_ui;
    Coroutine anim_coroutine;
    IconButton btn_light;
    IconButton btn_pump;
    Brace brace_plant;
    BitmapWidget plant;

    int32_t anim_w_brd = 70, anim_h_brd = 70;
    int32_t anim_x2_clip_light = 30, anim_y2_clip_light= 6;
    int32_t anim_x2_clip_soil = 20, anim_y2_clip_soil = 20;
    int32_t anim_x_water_pump = 42;
    int32_t anim_width_divider_line = 0;

    enum class ControlState {AUTO, ON};
    ControlState state_pump = ControlState::AUTO, state_lamp = ControlState::AUTO;
public:

    Greenhouse_App(PixelUI& ui, void* parameter);
    void onEnter(ExitCallback exitCallback) override;
    void onExit() override;
    bool handleInput(InputEvent event) override;
    void draw() override;
};
