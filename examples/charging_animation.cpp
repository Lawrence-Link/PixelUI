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

// --- USER DEFINED APP: charging animation

#include "core/app/IApplication.h"
#include "core/app/app_system.h"
#include <memory>

static const unsigned char image_Icon_Charging_bits[] = {0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xff,0xff,0xbf,0xff,0xff,0xdf,0xff,0xff,0xcf,0xff,0xff,0xe7,0xff,0xff,0xc3,0xff,0xff,0xe7,0xff,0xff,0xf3,0xff,0xff,0xfb,0xff,0xff,0xfd,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f};

// State machine
enum class ChargeState {
    LIGHTNING_AND_RING,  // Lightning appears + ring fills from 0 to percent
    SHRINK_RING,         // Ring shrinks to 0
    MOVE_LIGHTNING,      // Lightning moves left + number appears
    DONE
};

class ChargeDemo : public IApplication {
private:
    PixelUI& m_ui;

    // Animation variables
    int32_t lightIconSize = 0;
    int32_t batteryPercent_anim = 0;
    int32_t ringPercent = 0;         // Ring animation variable
    int32_t lightningOffsetX = 0;    // Lightning offset
    int32_t rectWidth = 10;           // Background rectangle width
    int batteryPercent = 50;       // Assumed battery level is 50%

    // State machine
    ChargeState state = ChargeState::LIGHTNING_AND_RING;
    unsigned long stateEnterTime = 0; // The time when each state is entered

public:
    ChargeDemo(PixelUI& ui, void* parameter) : m_ui(ui) {}

    // ---------------- Drawing function ----------------
    void draw() override {
        m_ui.markDirty();
        U8G2& display = m_ui.getU8G2();

        int centerX = 64 + lightningOffsetX;
        int centerY = 32;

        // Draw lightning
        drawChargingLightning(lightIconSize, centerX, centerY);
 
        // Draw the ring (animated)
        drawBatteryRing(display, 64, 32, 15, 2, ringPercent);

        // Draw the percentage number
        if (state == ChargeState::MOVE_LIGHTNING || state == ChargeState::DONE) {
            char buf[8];
            sprintf(buf, "%d%%", (int)batteryPercent_anim);
            display.setFont(u8g2_font_6x10_tf);
            display.drawStr(65, 36, buf);
        }

        // State machine scheduler
        updateState();
    }

    // ---------------- State machine scheduler ----------------
    void updateState() {
        switch(state) {
            case ChargeState::LIGHTNING_AND_RING:
                // Transition to the next state only after the initial animation is mostly complete.
                // 仅在初始动画基本完成后，才过渡到下一个状态。
                if (m_ui.getCurrentTime() - stateEnterTime > 1200) {
                    state = ChargeState::SHRINK_RING;
                    stateEnterTime = m_ui.getCurrentTime();
                    // Animate the ring shrinking to 0
                    // 动画圆环收缩到0
                    m_ui.animate(ringPercent, 0, 600, EasingType::EASE_OUT_CUBIC);
                }
                break;

            case ChargeState::SHRINK_RING:
                // Check if the ring animation is complete (ringPercent is close to 0).
                if (m_ui.getCurrentTime() - stateEnterTime > 900) {
                    state = ChargeState::MOVE_LIGHTNING;
                    stateEnterTime = m_ui.getCurrentTime();
                    // Animate lightning moving left, percentage text appearing, and background expanding
                    
                    m_ui.animate(lightningOffsetX, -10, 600, EasingType::EASE_OUT_CUBIC);
                    m_ui.animate(batteryPercent_anim, batteryPercent, 600, EasingType::EASE_OUT_CUBIC);
                    m_ui.animate(rectWidth, 90, 670, EasingType::EASE_OUT_CUBIC);
                }
                break;

            case ChargeState::MOVE_LIGHTNING:
                // Transition to DONE after the move animation is complete
                // 在移动动画完成后，过渡到DONE状态
                if (m_ui.getCurrentTime() - stateEnterTime > 2200) {
                    state = ChargeState::DONE;
                    stateEnterTime = m_ui.getCurrentTime();
                }
                break;

            case ChargeState::DONE:
                // Remain in the final state
                requestExit();
                break;
        }
    }

    // ---------------- Input handling ----------------
    bool handleInput(InputEvent event) override {
        requestExit(); // Any input exits the application
        return true;
    }

    // ---------------- Lifecycle ----------------
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);

        // Lightning size animation
        m_ui.animate(lightIconSize, 7, 400, EasingType::EASE_IN_CUBIC, PROTECTION::PROTECTED);

        // Ring animates from 0 to the battery percentage
        ringPercent = 0;
        m_ui.animate(ringPercent, batteryPercent, 600, EasingType::EASE_OUT_CUBIC);

        batteryPercent_anim = 0; // Percentage text is hidden initially
        lightningOffsetX = 0;    // Initial lightning position

        state = ChargeState::LIGHTNING_AND_RING;
        stateEnterTime = m_ui.getCurrentTime();

        m_ui.setContinousDraw(true);
        m_ui.markDirty();
    }

    void onExit() override {
        m_ui.setContinousDraw(false);
    }

private:
    // ---------------- Lightning drawing ----------------
    void drawChargingLightning(int size, int centerX, int centerY) {
        U8G2 &g = m_ui.getU8G2();
        int p1x = centerX + size * 0.4; int p1y = centerY - size * 0.6;
        int p2x = centerX - size * 0.1; int p2y = centerY - size * 0.1;
        int p3x = centerX + size * 0.35; int p3y = centerY - size * 0.1;
        int p4x = centerX - size * 0.35; int p4y = centerY + size * 0.1;
        int p5x = centerX + size * 0.1; int p5y = centerY + size * 0.1;
        int p6x = centerX - size * 0.4; int p6y = centerY + size * 0.6;

        g.drawLine(p1x, p1y, p4x, p4y);
        g.drawLine(p4x, p4y, p5x, p5y);
        g.drawLine(p5x, p5y, p6x, p6y);
        g.drawLine(p1x, p1y, p2x, p2y);
        g.drawLine(p2x, p2y, p3x, p3y);
        g.drawLine(p3x, p3y, p6x, p6y);
        g.drawLine(p1x, p1y, p6x, p6y);
    }

    // ---------------- Ring drawing ----------------
    void drawBatteryRing(U8G2 &u8g2, int x0, int y0, int radius, int thickness, int percent) {
        if (thickness < 1) thickness = 1;
        if (radius <= 0) return;
        if (percent <= 0) return;

        const uint8_t TOP = 64; // 12 o'clock
        uint8_t len = (uint8_t)((percent * 256UL) / 100UL);
        uint8_t arcStart = (uint8_t)(TOP - len);
        uint8_t arcEnd   = TOP;

        for (int w = 0; w < thickness; ++w) {
            int r = radius - w;
            if (r > 0) u8g2.drawArc(x0, y0, r, arcStart, arcEnd);
        }
    }
};

// ---------------- Application registration ----------------

AppItem charging_anim{
    .title = "Charging animation",
    .bitmap = image_Icon_Charging_bits,
    .createApp = [](PixelUI& ui, void* parameter) -> std::unique_ptr<IApplication> {
        return std::make_unique<ChargeDemo>(ui, parameter);
    },
};
