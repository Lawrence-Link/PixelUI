/**
 * @file app_counter.cpp
 * @brief Geiger Counter UI Demonstration Application.
 *
 * This file contains the implementation of the APP_COUNTER class,
 * a demonstration application for a Geiger counter user interface,
 * featuring various widgets like a histogram, brace display, and status icons.
 * It also handles an initial loading animation sequence.
 *
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
#include "core/app/IApplication.h"
#include "core/app/app_system.h"

#include <etl/memory.h>
#include <etl/stack.h>
#include "widgets/histogram/histogram.h"
#include "widgets/curve_chart/curve_chart.h"
#include "widgets/brace/brace.h"
#include "widgets/icon_button/icon_button.h"
#include "focus/focus.h"
#include "math.h"

// --- Bitmap definitions for UI elements ---

/** @brief Bitmap data for application info icon (12x12). */
static const unsigned char image_counter_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xde,0xff,0x7b,0x8e,0xff,0x71,0x87,0xff,0xe1,0x03,0xff,0xc0,0x03,0x7e,0xc0,0x01,0x7e,0x80,0x01,0x3c,0x80,0x01,0x3c,0x80,0x01,0x66,0x80,0x01,0xc3,0x80,0xff,0xc3,0xff,0xff,0xe7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc3,0xff,0xff,0xc3,0xff,0xff,0x00,0xff,0xff,0x00,0xff,0x7e,0x00,0x7e,0xfe,0x00,0x7f,0xfc,0xc3,0x3f,0xf0,0xff,0x0f
};

/** @brief Bitmap data for the background image (128x10). */
static const unsigned char image_Background_bits[] = {0xfe,0x01,0x00,0x00,0x00,0x00,0x00,0xe0,0xff,0xff,0xff,0x0f,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x7d,0x06,0x00,0x00,0x00,0x00,0x00,0x18,0xff,0xb7,0x55,0x31,0x00,0x00,0x00,0x00,0x81,0xfc,0xff,0xff,0xff,0xff,0xff,0x8f,0x00,0x00,0x00,0xe2,0xff,0xff,0xff,0x7f,0x3d,0x01,0x00,0x00,0x00,0x00,0x00,0x40,0xb6,0xea,0xff,0x04,0x00,0x00,0x00,0x80,0x41,0xfe,0xff,0xff,0xaa,0xfe,0xff,0x3f,0x01,0x00,0x00,0xf9,0xff,0xff,0xff,0xab,0x9f,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xf8,0xff,0x7f,0x02,0x00,0x00,0x00,0x80,0x20,0xff,0xff,0xff,0xff,0x55,0xfd,0x7f,0xfc,0xff,0xff,0x6c,0xff,0xff,0xff,0xb5,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x80,0x01,0x00,0x00,0x00,0x80,0x80,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03,0x00,0x00,0xff,0xff,0xff,0xff,0xff};

/** @brief Static data buffer for the histogram widget (25 values). */
static float s_static_data_buffer[25] = {
    0.1f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 0.9f, 0.7f, 0.5f, 0.3f,
    0.2f, 0.1f, 0.3f, 0.5f, 0.7f, 0.9f, 1.0f, 0.8f, 0.6f, 0.4f,
    0.2f, 0.1f, 0.2f, 0.3f, 0.4f
};

/** @brief Bitmap data for the sound ON icon (7x7). */
static const unsigned char image_SOUND_ON_bits[] = {0x24,0x46,0x57,0x57,0x57,0x46,0x24};
/** @brief Bitmap data for the sound OFF icon (7x7). */
static const unsigned char image_SOUND_OFF_bits[] = {0x04,0x06,0x57,0x27,0x57,0x06,0x04};
/** @brief Bitmap data for the alarm bell icon (6x7). */
static const unsigned char image_BELL_bits[] = {0x20,0x18,0x3c,0x3e,0x1f,0x1c,0x12};
/** @brief Bitmap data for the alert icon (9x7). */
static const unsigned char image_Alert_bits[] = {0x10,0x00,0x38,0x00,0x28,0x00,0x6c,0x00,0x6c,0x00,0xfe,0x00,0xef,0x01};
/** @brief Bitmap data for the battery full icon (10x6). */
static const unsigned char image_BAT_FULL_bits[] = {0xff,0x01,0xff,0x03,0xff,0x03,0xff,0x03,0xff,0x03,0xff,0x01};
/** @brief Bitmap data for the battery 75% icon (10x6). */
static const unsigned char image_BAT_75_bits[] = {0xff,0x01,0x3f,0x03,0x3f,0x03,0x3f,0x03,0x3f,0x03,0xff,0x01};
/** @brief Bitmap data for the battery 50% icon (10x6). */
static const unsigned char image_BAT_50_bits[] = {0xff,0x01,0x1f,0x03,0x1f,0x03,0x1f,0x03,0x1f,0x03,0xff,0x01};
/** @brief Bitmap data for the battery 25% icon (10x6). */
static const unsigned char image_BAT_25_bits[] = {0xff,0x01,0x07,0x03,0x07,0x03,0x07,0x03,0x07,0x03,0xff,0x01};
/** @brief Bitmap data for the battery empty icon (10x6). */
static const unsigned char image_BAT_empty_bits[] = {0xff,0x01,0x01,0x03,0x01,0x03,0x01,0x03,0x01,0x03,0xff,0x01};

// --- USER DEFINED APP: A Geiger counter UI demo ---

/**
 * @brief Implements the Geiger Counter UI demonstration application.
 *
 * This application displays a mock-up of a Geiger counter interface,
 * including a histogram for radiation levels, a brace for the main reading,
 * and status icons for battery, sound, and alarms. It also features a
 * controlled loading animation sequence on startup.
 */
class APP_COUNTER: public IApplication {
private:
    /** @brief Reference to the main UI context. */
    PixelUI& m_ui;
    /** @brief Caller-owned chart storage sized for the expanded width. */
    float histogramBuffer[76]{};
    /** @brief Widget for displaying historical data. */
    CurveChart histogram;
    /** @brief Widget for displaying a main, bracketed value. */
    Brace brace;
    /** @brief Icon button for battery status. */
    IconButton icon_battery;
    /** @brief Icon button for alert status. */
    IconButton icon_alert;
    /** @brief Icon button for sounding status. */
    IconButton icon_sounding;
    /** @brief Icon button for alarm status. */
    IconButton icon_alarm;

    /**
     * @brief State machine for the application's initial loading animation sequence.
     */
    enum class LoadState {
        INIT,          ///< Initial state before loading starts.
        BRACE_LOADING, ///< Execute brace and battery icon loading animations.
        WAIT_HISTO,    ///< Short delay before histogram animation.
        HISTO_LOADING, ///< Execute histogram and other icon loading animations.
        DONE           ///< All loading animations complete.
    } loadState = LoadState::INIT;

    /** @brief Stores the timestamp when the current state was entered. */
    uint32_t state_timestamp = 0;
    /** @brief Flag to ensure the initial setup and animations run only once. */
    bool first_time = false;

    // Animation related variables (used as animation targets)
    /** @brief Animation target for the measurement mark box width. */
    int32_t anim_mark_m = 0;
    /** @brief Animation target for the background clipping width. */
    int32_t anim_bg = 0;
    /** @brief Animation target for the status text X position. */
    int32_t anim_status_x = -27;

public:
    /**
     * @brief Constructor for APP_COUNTER.
     * @param ui Reference to the PixelUI instance.
     */
    APP_COUNTER(PixelUI& ui, void* parameter = nullptr) :
    m_ui(ui),
    // Initialize all member widgets with the UI reference
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
    icon_battery(ui, 14, 2, 10, 6),
    icon_alert(ui, 28, 1, 9, 7),
    icon_sounding(ui, 40, 1, 7, 7),
    icon_alarm(ui, 51, 1, 6, 7)
    {}

    /**
     * @brief Setup function called when the application is entered.
     * @param cb Callback function to be executed upon application exit.
     */
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        // Enable continuous drawing for smooth animations
        m_ui.setContinousDraw(true);

        // --- HISTOGRAM SETUP ---
        // Define expansion behaviour (to full screen for stats view)
        // histogram.setExpand(EXPAND_BASE::BOTTOM_RIGHT, 76, 63);

        // --- BRACE SETUP ---
        // Set custom content drawing function for the brace widget
        brace.setDrawContentFunction([this]() { braceContent(); });

        // --- ICON BUTTON SETUP ---
        // Battery Icon
        icon_battery.setSource(image_BAT_75_bits);
        // Sounding Icon
        icon_sounding.setSource(image_SOUND_OFF_bits);
        // Alert Icon
        icon_alert.setSource(image_Alert_bits);
        // Alarm Icon
        icon_alarm.setSource(image_BELL_bits);

        // --- FOCUS MANAGEMENT SETUP ---
        // Add interactive widgets to the focus manager
        m_ui.addWidgetToFocusManager(&brace);
        m_ui.addWidgetToFocusManager(&histogram);

        // Initialize state machine and first-time flag
        loadState = LoadState::INIT;
        first_time = false;
    }

    /**
     * @brief Custom content drawing for the Brace widget.
     *
     * Draws the main radiation reading "10.00 uSv/h" and a "Max" label.
     */
    void braceContent() {
        U8G2& u8g2 = m_ui.getU8G2();
        // Draw the main value and unit
        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(30, 54, "10.00");
        u8g2.drawStr(31, 61, "uSv/h");
        // Draw the "Max" box
        u8g2.drawRBox(8, 50, 20, 10, 2);
        // Set draw color to background (0) to invert the text color inside the box
        u8g2.setDrawColor(0);
        u8g2.drawStr(11, 58, "Max");
        // Restore draw color to foreground (1)
        u8g2.setDrawColor(1);
    }

    /**
     * @brief Main drawing function, called continuously.
     *
     * Handles the loading state machine and draws all UI elements.
     */
    void draw() override {
        // --- INITIAL ANIMATION SETUP ---
        if (!first_time) {
            // Start animations for the measurement mark and background reveal
            m_ui.animate(anim_mark_m, 23, 300, EasingType::EASE_OUT_QUAD, PROTECTION::PROTECTED);
            m_ui.animate(anim_bg, 128, 500, EasingType::EASE_IN_OUT_CUBIC, PROTECTION::PROTECTED);

            // Transition to the first loading state and record the time
            loadState = LoadState::BRACE_LOADING;
            state_timestamp = m_ui.getCurrentTime();
            first_time = true;
        }

        // --- LOADING STATE MACHINE ---
        switch (loadState) {
            case LoadState::BRACE_LOADING:
                // Start loading animations for brace and related icons
                brace.onLoad();
                icon_battery.onLoad();
                icon_alert.onLoad();
                // Move to the wait state
                loadState = LoadState::WAIT_HISTO;
                state_timestamp = m_ui.getCurrentTime();
                break;
            case LoadState::WAIT_HISTO:
                // Wait for a short duration (80ms)
                if (m_ui.getCurrentTime() - state_timestamp >= 80) {
                    loadState = LoadState::HISTO_LOADING;
                }
                break;
            case LoadState::HISTO_LOADING:
                // Start loading animations for histogram and other icons
                histogram.onLoad();

                for (int i = 0; i <= 100; i++)
                {histogram.addData(sin(0.3*i)+3);}

                icon_sounding.onLoad();
                icon_alarm.onLoad();
                // All loading is complete, move to DONE state
                loadState = LoadState::DONE;
                // Start animation to slide the "PLEASE WAIT" status text into view
                m_ui.animate(anim_status_x, 29, 450, EasingType::EASE_OUT_CUBIC, PROTECTION::PROTECTED);
                break;
            case LoadState::DONE:
                // Application is fully loaded, wait for user input or exit
                break;
            default:
                break;
        }

        // --- UI DRAWING ---
        U8G2& u8g2 = m_ui.getU8G2();

        // Draw seperation with animated clipping
        u8g2.setClipWindow(0, 7, anim_bg, 18);
        u8g2.drawXBM(0, 7, 128, 10, image_Background_bits);
        u8g2.setMaxClipWindow();

        // Draw "MEAS" label
        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(5, 42, "MEAS");
        
        // Draw status text ("PLEASE WAIT") with animated clipping
        u8g2.setClipWindow(29, 36, 83, 42);
        u8g2.drawStr(anim_status_x, 42, "PLEASE WAIT");
        u8g2.setMaxClipWindow();

        // Draw animated measurement mark box (color 2 is XOR mode)
        u8g2.setDrawColor(2);
        u8g2.drawBox(3, 35, anim_mark_m, 8);
        u8g2.setDrawColor(1); // Restore to foreground color

        // Draw placeholder reading
        u8g2.setFont(u8g2_font_profont17_tr);
        u8g2.drawStr(3, 31, "-.-- uSv/h");

        // Draw count data labels
        u8g2.setFont(u8g2_font_4x6_tr);
        u8g2.drawStr(100, 32, "CPM");
        u8g2.drawStr(100, 39, "0000");
        // histogram.setData(s_static_data_buffer, 25, 0); // deprecated method

        // Draw all widgets
        icon_sounding.draw();
        icon_alarm.draw();
        icon_alert.draw();
        icon_battery.draw();
        brace.draw();

        // Conditional drawing for histogram expanded view (STATS screen)
        if (histogram.isExpanded()) {
            u8g2.clearBuffer(); // Clear the buffer for the stats view
            u8g2.drawStr(3, 10, "<STATS>");
            u8g2.drawStr(3, 20, "Max:");
            u8g2.drawStr(3, 30, "1.45uSv/h");
            u8g2.drawStr(3, 40, "Min:");
            u8g2.drawStr(3, 50, "0.25uSv/h");
        }
        histogram.draw();
    }

    /**
     * @brief Handles input events (e.g., button presses).
     * @param event The input event received.
     * @return true if the event was handled, false otherwise.
     */
    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit(); // Request to close the application
        } 
        return true; // Standard app input handling is always true
    }

    /**
     * @brief Cleanup function called when the application exits.
     */
    void onExit() override {
        // Stop all ongoing animation
        m_ui.clearAllAnimations();
        // Stop continuous drawing to save power
        m_ui.setContinousDraw(false);
    }
};

// ---------------- Application registration ----------------
/**
 * @brief Application registration block.
 *
 * Registers the APP_COUNTER class with the application system,
 * providing its title, icon bitmap, and a factory function for creation.
 */

AppItem counter_app = AppItem::make<APP_COUNTER>("COUNTER", image_counter_bits);
