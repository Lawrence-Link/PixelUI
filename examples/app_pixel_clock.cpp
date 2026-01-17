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
#include <memory>
#include <ctime>

/** @brief Bitmap data for clock application icon (24x24). */
static const unsigned char image_clock_bits[] = {
    0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0x81,0xff,0x0f,0x00,0xf0,
    0x07,0x00,0xe0,0x07,0x00,0xe0,0x03,0x00,0xc0,0x03,0x3c,0xc0,0x03,0x3c,0xc0,0x03,0x3c,0xc0,
    0x03,0x00,0xc0,0x07,0x00,0xe0,0x07,0x00,0xe0,0x0f,0x00,0xf0,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};

/**
 * @brief Structure to hold scrolling animation state for a single digit.
 */
struct DigitState {
    int32_t currentValue;     ///< Current displayed value (0-9)
    int32_t offsetY;          ///< Vertical offset for animation (in pixels)
    bool isAnimating;         ///< Whether this digit is currently animating
};

/**
 * @brief Pixel Clock application with scrolling digit animations.
 */
class APP_PIXEL_CLOCK : public IApplication {
private:
    PixelUI& m_ui;
    
    // Digit states for HH:MM:SS
    DigitState hourTens;
    DigitState hourOnes;
    DigitState minuteTens;
    DigitState minuteOnes;
    DigitState secondTens;
    DigitState secondOnes;
    
    // Time synchronization
    uint32_t lastTickTime = 0;
    uint32_t lastSyncTime = 0;
    static constexpr uint32_t TICK_INTERVAL = 1000; // Tick every 1 second
    static constexpr uint32_t SYNC_INTERVAL = 60000; // Sync every 60 seconds
    bool isInitialized = false;
    
    // Animation parameters
    static constexpr uint32_t DIGIT_ANIM_DURATION = 300; // milliseconds
    static constexpr int32_t DIGIT_HEIGHT_LARGE = 32; // Height for large font
    static constexpr int32_t DIGIT_HEIGHT_SMALL = 16; // Height for small font
    
public:
    APP_PIXEL_CLOCK(PixelUI& ui, void* parameter) : m_ui(ui) {
        resetDigitState(hourTens);
        resetDigitState(hourOnes);
        resetDigitState(minuteTens);
        resetDigitState(minuteOnes);
        resetDigitState(secondTens);
        resetDigitState(secondOnes);
    }
    
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        m_ui.setContinousDraw(true);
        
        // Synchronize with system time on startup
        isInitialized = false;
        uint32_t currentTime = m_ui.getCurrentTime();
        lastTickTime = currentTime;
        lastSyncTime = currentTime;
        syncWithSystemTime();
    }
    
    void resetDigitState(DigitState& digit) {
        digit.currentValue = 0;
        digit.offsetY = 0;
        digit.isAnimating = false;
    }
    
    /**
     * @brief Start animation for a digit from current to target value.
     */
    void animateDigit(DigitState& digit, int newValue, bool isLarge = true) {
        // Don't animate if already animating or value hasn't changed
        if (digit.isAnimating || digit.currentValue == newValue) {
            return;
        }
        
        // Immediately update to new value
        digit.currentValue = newValue;
        
        // Set initial offset based on font size
        int32_t digitHeight = isLarge ? DIGIT_HEIGHT_LARGE : DIGIT_HEIGHT_SMALL;
        digit.offsetY = digitHeight;
        digit.isAnimating = true;
        
        // Animate offset back to 0
        m_ui.animate(digit.offsetY, 0, DIGIT_ANIM_DURATION, EasingType::EASE_OUT_QUAD);
        
        m_ui.markDirty();
    }
    
    /**
     * @brief Synchronize with system time (only called on startup and periodic sync).
     */
    void syncWithSystemTime() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        
        int hour = timeinfo->tm_hour;
        int minute = timeinfo->tm_min;
        int second = timeinfo->tm_sec;
        
        // Set all digits without animation
        hourTens.currentValue = hour / 10;
        hourOnes.currentValue = hour % 10;
        minuteTens.currentValue = minute / 10;
        minuteOnes.currentValue = minute % 10;
        secondTens.currentValue = second / 10;
        secondOnes.currentValue = second % 10;
        
        isInitialized = true;
    }
    
    /**
     * @brief Increment the clock by one second (event-driven).
     */
    void incrementSecond() {
        // Calculate new values before animating
        int currentSecondOnes = secondOnes.currentValue;
        int currentSecondTens = secondTens.currentValue;
        int newSecondOnes = (currentSecondOnes + 1) % 10;
        int newSecondTens = currentSecondTens;
        bool minuteCarry = false;
        
        // Check if we need to increment seconds tens
        if (newSecondOnes == 0) {
            newSecondTens = (currentSecondTens + 1) % 6;
            // Check if we need to increment minutes (60 seconds passed)
            if (newSecondTens == 0) {
                minuteCarry = true;
            }
        }
        
        // Now animate the changes
        animateDigit(secondOnes, newSecondOnes, false); // Small font
        if (newSecondTens != currentSecondTens) {
            animateDigit(secondTens, newSecondTens, false); // Small font
        }
        if (minuteCarry) {
            incrementMinute();
        }
    }
    
    /**
     * @brief Increment the minutes.
     */
    void incrementMinute() {
        // Calculate new values before animating
        int currentMinuteOnes = minuteOnes.currentValue;
        int currentMinuteTens = minuteTens.currentValue;
        int newMinuteOnes = (currentMinuteOnes + 1) % 10;
        int newMinuteTens = currentMinuteTens;
        bool hourCarry = false;
        
        // Check if we need to increment minutes tens
        if (newMinuteOnes == 0) {
            newMinuteTens = (currentMinuteTens + 1) % 6;
            // Check if we need to increment hours (60 minutes passed)
            if (newMinuteTens == 0) {
                hourCarry = true;
            }
        }
        
        // Now animate the changes
        animateDigit(minuteOnes, newMinuteOnes, true); // Large font
        if (newMinuteTens != currentMinuteTens) {
            animateDigit(minuteTens, newMinuteTens, true); // Large font
        }
        if (hourCarry) {
            incrementHour();
        }
    }
    
    /**
     * @brief Increment the hours.
     */
    void incrementHour() {
        // Calculate current hour value before any animation
        int currentHour = hourTens.currentValue * 10 + hourOnes.currentValue;
        int newHour = (currentHour + 1) % 24;
        int newHourTens = newHour / 10;
        int newHourOnes = newHour % 10;
        
        // Now animate both digits
        animateDigit(hourTens, newHourTens, true); // Large font
        animateDigit(hourOnes, newHourOnes, true); // Large font
    }
    
    /**
     * @brief Draw a single digit with scrolling effect.
     */
    void drawScrollingDigit(int16_t x, int16_t y, DigitState& digit, bool useLargeFont, int16_t width, int16_t height) {
        U8G2& u8g2 = m_ui.getU8G2();
        
        int32_t digitHeight = useLargeFont ? DIGIT_HEIGHT_LARGE : DIGIT_HEIGHT_SMALL;
        
        if (useLargeFont) {
            u8g2.setFont(u8g2_font_freedoomr25_tn);
        } else {
            u8g2.setFont(u8g2_font_freedoomr10_tu);
        }
        
        // Set clipping window - extend downward to ensure bottom digits are clipped
        u8g2.setClipWindow(x, y - height, x + width, y + 4);
        
        // Draw previous, current, and next digit (like NumScroll)
        char buf[2] = {0};
        
        // Draw previous digit (above, only visible when scrolling down)
        int prevValue = (digit.currentValue - 1 + 10) % 10;
        buf[0] = '0' + prevValue;
        int16_t prevY = y - digitHeight + digit.offsetY;
        u8g2.drawStr(x, prevY, buf);
        
        // Draw current digit (centered, with offset)
        buf[0] = '0' + digit.currentValue;
        int16_t currY = y + digit.offsetY;
        u8g2.drawStr(x, currY, buf);
        
        // Draw next digit (below, only visible when scrolling up)
        int nextValue = (digit.currentValue + 1) % 10;
        buf[0] = '0' + nextValue;
        int16_t nextY = y + digitHeight + digit.offsetY;
        u8g2.drawStr(x, nextY, buf);
        
        // Reset clipping window
        u8g2.setMaxClipWindow();
        
        // Check if animation is complete
        if (digit.isAnimating && digit.offsetY == 0) {
            digit.isAnimating = false;
        }
    }
    
    void draw() override {
        U8G2& u8g2 = m_ui.getU8G2();
        
        if (isInitialized) {
            uint32_t currentTime = m_ui.getCurrentTime();
            
            // Sync with system time every 60 seconds to prevent drift
            if (currentTime - lastSyncTime >= SYNC_INTERVAL) {
                syncWithSystemTime();
                lastSyncTime = currentTime;
                lastTickTime = currentTime; // Reset tick timer after sync
            }
            // Tick every second (event-driven)
            else if (currentTime - lastTickTime >= TICK_INTERVAL) {
                incrementSecond();
                lastTickTime = currentTime;
            }
        }
        
        // Update all digit animations
        // (Animations are now handled by PixelUI's animation system)
        
        // Position: bottom-right area
        // Screen is 128x64
        // Large font: ~20 pixels wide, ~26 pixels tall
        // Small font: ~10 pixels wide, ~12 pixels tall
        
        int16_t baseY = 60; // Near bottom
        int16_t secondBaseY = baseY;
        
        // Calculate positions from right to left
        // Seconds are at the rightmost position
        int16_t secondX = u8g2.getDisplayWidth() - 22; // Leave some margin from right edge
        int16_t minuteX = secondX - 42; // Before seconds
        int16_t hourX = minuteX - 40;   // Before minutes
        
        // Font dimensions for clipping
        int16_t largeDigitWidth = 20;
        int16_t largeDigitHeight = 26;
        int16_t smallDigitWidth = 10;
        int16_t smallDigitHeight = 12;
        
        // Draw hours (large font)
        drawScrollingDigit(hourX, baseY, hourTens, true, largeDigitWidth, largeDigitHeight);
        drawScrollingDigit(hourX + 20, baseY, hourOnes, true, largeDigitWidth, largeDigitHeight);
        
        // Draw minutes (large font) - no colon before
        drawScrollingDigit(minuteX, baseY, minuteTens, true, largeDigitWidth, largeDigitHeight);
        drawScrollingDigit(minuteX + 20, baseY, minuteOnes, true, largeDigitWidth, largeDigitHeight);
        
        // Draw seconds (small font) - no colon before
        drawScrollingDigit(secondX, secondBaseY, secondTens, false, smallDigitWidth, smallDigitHeight);
        drawScrollingDigit(secondX + 10, secondBaseY, secondOnes, false, smallDigitWidth, smallDigitHeight);
    }
    
    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit();
            return true;
        }
        return false;
    }
    
    void onExit() override {
        m_ui.setContinousDraw(false);
    }
};

// ---------------- Application registration ----------------
#if USE_STATIC_APP_REGISTER_ENABLED
static AppRegistrar pixel_clock_app({
    .title = "Clock",
    .bitmap = image_clock_bits,
    .createApp = [](PixelUI& ui) -> std::unique_ptr<IApplication> {
        return std::make_unique<APP_PIXEL_CLOCK>(ui);
    },
    .order = 4
});
#else
AppItem pixel_clock_app{
    .title = "Clock",
    .bitmap = image_clock_bits,
    .createApp = [](PixelUI& ui, void* parameter) -> std::unique_ptr<IApplication> {
        return std::make_unique<APP_PIXEL_CLOCK>(ui, parameter);
    },
};
#endif
