// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2025, Lawrence Link

#pragma once

#include <cstdint>

#include "core/app/IApplication.h"
#include "counter_reading_widget.h"
#include "widgets/brace/brace.h"
#include "widgets/curve_chart/curve_chart.h"
#include "widgets/icon_button/icon_button.h"

class PixelUI;

class APP_COUNTER : public IApplication {
public:
    explicit APP_COUNTER(PixelUI& ui, void* parameter = nullptr);

    void onEnter(ExitCallback cb) override;
    void draw() override;
    bool handleInput(InputEvent event) override;
    void onExit() override;

private:
    enum class LoadState {
        INIT,
        BRACE_LOADING,
        WAIT_HISTO,
        HISTO_LOADING,
        DONE
    };

    PixelUI& m_ui;
    float histogramBuffer[76]{};
    CurveChart histogram;
    Brace brace;
    CounterReadingWidget brace_content;
    IconButton icon_battery;
    IconButton icon_alert;
    IconButton icon_sounding;
    IconButton icon_alarm;

    LoadState loadState = LoadState::INIT;
    uint32_t state_timestamp = 0;
    bool first_time = false;
    int32_t anim_mark_m = 0;
    int32_t anim_bg = 0;
    int32_t anim_status_x = -27;
};
