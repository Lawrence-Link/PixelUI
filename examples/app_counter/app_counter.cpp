// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2025, Lawrence Link

#include "app_counter.h"

#include <cmath>

#include "PixelUI.h"
#include "app_resources.h"

APP_COUNTER::APP_COUNTER(PixelUI& ui, void* parameter) :
    m_ui(ui),
    histogram(
        ui,
        69,
        45,
        56,
        18,
        histogramBuffer,
        ChartExpandSize<76, 63>{},
        EXPAND_BASE::BOTTOM_RIGHT,
        "Curve"),
    brace(ui, 3, 45, 56, 18),
    brace_content(ui),
    icon_battery(ui, 14, 2, 10, 6),
    icon_alert(ui, 28, 1, 9, 7),
    icon_sounding(ui, 40, 1, 7, 7),
    icon_alarm(ui, 51, 1, 6, 7) {
    (void)parameter;
    brace.addChild(brace_content);
}

void APP_COUNTER::onEnter(ExitCallback cb) {
    IApplication::onEnter(cb);

    icon_battery.setSource(app_counter_resources::battery_75_icon);
    icon_sounding.setSource(app_counter_resources::sound_off_icon);
    icon_alert.setSource(app_counter_resources::alert_icon);
    icon_alarm.setSource(app_counter_resources::bell_icon);

    brace.setFocusable(true);
    histogram.setFocusable(true);
    m_ui.addWidgetToFocusManager(&brace);
    m_ui.addWidgetToFocusManager(&histogram);

    loadState = LoadState::INIT;
    first_time = false;
}

void APP_COUNTER::draw() {
    if (!first_time) {
        m_ui.animate(anim_mark_m, 23, 300, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
        m_ui.animate(anim_bg, 128, 500, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);
        loadState = LoadState::BRACE_LOADING;
        state_timestamp = m_ui.getCurrentTime();
        first_time = true;
    }

    switch (loadState) {
        case LoadState::BRACE_LOADING:
            brace.onLoad();
            icon_battery.onLoad();
            icon_alert.onLoad();
            loadState = LoadState::WAIT_HISTO;
            state_timestamp = m_ui.getCurrentTime();
            break;
        case LoadState::WAIT_HISTO:
            if (m_ui.getCurrentTime() - state_timestamp >= 80) {
                loadState = LoadState::HISTO_LOADING;
            }
            break;
        case LoadState::HISTO_LOADING:
            histogram.onLoad();
            for (int i = 0; i <= 100; ++i) {
                histogram.addData(std::sin(0.3 * i) + 3);
            }
            icon_sounding.onLoad();
            icon_alarm.onLoad();
            loadState = LoadState::DONE;
            m_ui.animate(anim_status_x, 29, 450, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
            break;
        case LoadState::INIT:
        case LoadState::DONE:
            break;
    }

    Canvas& canvas = m_ui.getCanvas();
    if (!histogram.isExpanded()) {
        canvas.setFont(u8g2_font_5x7_tr);
        canvas.drawStr(5, 42, "MEAS");

        canvas.setClipWindow(29, 36, 83, 42);
        canvas.drawStr(anim_status_x, 42, "PLEASE WAIT");
        canvas.setMaxClipWindow();

        canvas.setDrawColor(2);
        canvas.drawBox(3, 35, anim_mark_m, 8);
        canvas.setDrawColor(1);

        canvas.setFont(u8g2_font_profont17_tr);
        canvas.drawStr(3, 31, "-.-- uSv/h");

        canvas.setFont(u8g2_font_4x6_tr);
        canvas.drawStr(100, 32, "CPM");
        canvas.drawStr(100, 39, "0000");

        canvas.setClipWindow(0, 7, anim_bg, 18);
        canvas.drawXBM(0, 7, 128, 10, app_counter_resources::background);
        canvas.setMaxClipWindow();

        icon_sounding.draw();
        icon_alarm.draw();
        icon_alert.draw();
        icon_battery.draw();
        brace.draw();
    } else {
        canvas.setFontMode(1);
        canvas.setDrawColor(1);
        canvas.drawBox(0, 31, 47, 8);
        canvas.drawBox(0, 0, 47, 8);
        canvas.setDrawColor(2);
        canvas.setFont(u8g2_font_5x7_tr);
        canvas.drawStr(6, 7, "MAXIMUM");
        canvas.drawStr(6, 38, "AVERAGE");
        canvas.setDrawColor(1);
        canvas.drawStr(17, 28, "CPM");
        canvas.drawStr(17, 60, "CPM");
        canvas.setFont(u8g2_font_profont11_tr);
        canvas.drawStr(3, 18, "MAX_VAL");
        canvas.drawStr(3, 49, "AVG_VAL");
    }
    histogram.draw();
}

bool APP_COUNTER::handleInput(InputEvent event) {
    if (event == InputEvent::BACK) {
        requestExit();
    }
    return true;
}

void APP_COUNTER::onExit() {
    m_ui.clearAllAnimations();
}
