// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2025, Lawrence Link

#pragma once

#include <cstdint>

#include "core/app/IApplication.h"
#include "counter_reading_widget.h"
#include "widgets/brace/brace.h"
#include "widgets/curve_chart/curve_chart.h"
#include "widgets/histogram/histogram.h"
#include "widgets/icon_button/icon_button.h"

class PixelUI;

class APP_COUNTER : public IApplication {
public:
    /**
     * @brief Creates the counter demonstration application and its widgets.
     * @param ui UI instance used for rendering, animation, and focus handling.
     * @param parameter Reserved application parameter; currently ignored.
     */
    explicit APP_COUNTER(PixelUI& ui, void* parameter = nullptr);

    /**
     * @brief Initializes widget resources, focus registration, and load state.
     * @param cb Callback used to request removal of the application.
     */
    void onEnter(ExitCallback cb) override;

    /** @brief Advances the staged entrance sequence and draws the application. */
    void draw() override;

    /**
     * @brief Handles application input and requests exit for the back event.
     * @param event Input event to process.
     * @return true because this demonstration consumes every input event.
     */
    bool handleInput(InputEvent event) override;

    /** @brief Cancels animations owned by the demonstration. */
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
    StaticChartSeries<76> histogramSeries;
    Histogram histogram;
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
