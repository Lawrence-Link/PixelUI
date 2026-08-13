/*
 * PixelUI.cpp - PixelUI main class implementation
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

#include "PixelUI.h"
#include "core/ViewManager/ViewManager.h"
#include "core/animation/animation.h"
#include "ui/Popup/PopupProgress.h"
#include "ui/Popup/PopupInfo.h"
#include "ui/Popup/PopupValue4Digits.h"
#include "core/coroutine/Coroutine.h"
#include "focus/focus.h"
#include <inttypes.h>

/**
 * @brief Construct a PixelUI instance with a U8G2 display reference
 * @param u8g2 Reference to the U8G2 display object
 *
 * Initializes core subsystems: ViewManager, AnimationManager, PopupManager, and CoroutineScheduler.
 */
PixelUI::PixelUI(U8G2& u8g2)
    : u8g2_(u8g2), m_popupManager(*this), _currentTime(0) {
    m_viewManagerPtr.reset(new ViewManager(*this));
    m_coroutineSchedulerPtr.reset(new CoroutineScheduler(*this));
    m_focusManagerPtr.reset(new FocusManager(*this));
}

PixelUI::~PixelUI() {
    // Applications may reference every other manager, so destroy them first.
    m_viewManagerPtr.reset();
}

/**
 * @brief Add a coroutine to the scheduler
 * @param coroutine Non-owning pointer to the Coroutine object
 */
void PixelUI::addCoroutine(Coroutine* coroutine) { 
    m_coroutineSchedulerPtr->addCoroutine(coroutine); 
}

/**
 * @brief Remove a coroutine from the scheduler
 * @param coroutine Non-owning pointer to the Coroutine object
 */
void PixelUI::removeCoroutine(Coroutine* coroutine) { 
    m_coroutineSchedulerPtr->removeCoroutine(coroutine); 
}

/**
 * @brief Placeholder initialization function
 *
 * Currently empty; kept for API consistency and future expansion.
 */
void PixelUI::begin() { }

/**
 * @brief Heartbeat function to update all subsystems
 * @param ms Milliseconds elapsed since last heartbeat
 *
 * Updates animation manager, popup manager, and coroutine scheduler.
 */
void PixelUI::Heartbeat(uint32_t ms) {
    _currentTime += ms;
    update_symbol_.store(1);
}

bool PixelUI::animateCallback(
    int32_t startValue,
    int32_t endValue,
    uint32_t duration,
    EasingType easing,
    ValueCallback callback,
    PROTECTION protection) {
    return m_animationManager.emplace(
        startValue,
        endValue,
        duration,
        easing,
        etl::move(callback),
        protection,
        _currentTime);
}

/**
 * @brief Animate a single integer value with optional protection
 */
bool PixelUI::animate(int32_t& value, int32_t targetValue, uint32_t duration,
                      EasingType easing, PROTECTION prot) {

    return animateCallback(
        value,
        targetValue,
        duration,
        easing,
        [&value](int32_t currentValue) { value = currentValue; },
        prot);
}

/**
 * @brief Animate two integer values simultaneously
 */
bool PixelUI::animate(int32_t& x, int32_t& y, int32_t targetX, int32_t targetY,
                      uint32_t duration, EasingType easing, PROTECTION prot) {

    if (m_animationManager.available() < 2U) {
        return false;
    }

    const bool xAdded = animateCallback(
        x, targetX, duration, easing, [&x](int32_t val) { x = val; }, prot);
    const bool yAdded = animateCallback(
        y, targetY, duration, easing, [&y](int32_t val) { y = val; }, prot);
    return xAdded && yAdded;
}
/**
 * @brief Add a widget to the FocusManager
 * @param w Pointer to the widget to add
 */
bool PixelUI::addWidgetToFocusManager(IWidget* w) {
    return m_focusManagerPtr->addWidget(w);
}
/**
 * @brief Clear all widgets from the FocusManager
 */
void PixelUI::clearFocusManager() {
    m_focusManagerPtr->clear();
}

size_t PixelUI::getFocusedWidgetCount() const {
    return m_focusManagerPtr->widgetCount();
}
/**
 * @brief Handle input event
 * @param event InputEvent to handle
 *
 * Passes the input event to the FocusManager and the registered input callback.
 */

void PixelUI::handleInput(InputEvent event) {
    if (m_focusManagerPtr) 
    {
        if (m_focusManagerPtr->handleInput(event)) return;
    }
    if (inputCallback_) inputCallback_(event);
}

/**
 * @brief Render the current drawable and active popups
 *
 * Handles optional fading effects and calls the refresh callback if set.
 */
void PixelUI::renderer() {
    if (m_viewManagerPtr->isTransitioning()) return;

    if (update_symbol_.load()) { // check for update before rendering context
        update_symbol_.store(0) ;
        m_popupManager.updatePopups(_currentTime);
        m_coroutineSchedulerPtr->update(_currentTime);
        m_animationManager.update(_currentTime);
    }

    static uint8_t lastPopupCount = 0;
    uint8_t currentPopupCount = static_cast<uint8_t>(m_popupManager.getPopupCounts());
    if (currentPopupCount != lastPopupCount) { markDirty(); lastPopupCount = currentPopupCount; }

    if (activeAnimationCount() || isContinousRefreshEnabled()) { markDirty(); }

    if (!isFading_) {
        u8g2_.clearBuffer();
        if (currentDrawable_) currentDrawable_->draw();
        m_popupManager.drawPopups();
        m_focusManagerPtr->draw();
        u8g2_.sendBuffer();
        if (m_refresh_callback) m_refresh_callback();
        isDirty_ = false;
    } else {
        if (m_fadeStep == 0) {
            u8g2_.clearBuffer();
            if (currentDrawable_) currentDrawable_->draw();
            m_popupManager.drawPopups();
            u8g2_.sendBuffer();
            if (m_refresh_callback) m_refresh_callback();
            m_fadeStep = 1;
            m_lastFadeTime = getCurrentTime();
            return;
        }
        if (m_fadeStep >= 1 && m_fadeStep <= 4) {
            if (getCurrentTime() - m_lastFadeTime < 40) return;

            uint8_t *buf_ptr = u8g2_.getBufferPtr();
            uint16_t buf_len = 1024;
            switch (m_fadeStep) {
                case 1: for (uint16_t i=0;i<buf_len;i++) if (i%2) buf_ptr[i] &= 0xAA; break;
                case 2: for (uint16_t i=0;i<buf_len;i++) if (i%2) buf_ptr[i] &= 0x00; break;
                case 3: for (uint16_t i=0;i<buf_len;i++) if (!(i%2)) buf_ptr[i] &= 0x55; break;
                case 4: for (uint16_t i=0;i<buf_len;i++) if (!(i%2)) buf_ptr[i] &= 0x00; break;
            }
            u8g2_.sendBuffer();
            if (m_refresh_callback) m_refresh_callback();
            m_lastFadeTime = getCurrentTime();
            m_fadeStep++;
            if (m_fadeStep > 4) { isFading_ = false; m_fadeStep = 0; }
        }
    }
}

/**
 * @brief Show a progress popup
 */
bool PixelUI::showPopupProgress(int32_t& value, int32_t minValue, int32_t maxValue,
                                const char* title, uint16_t width, uint16_t height,
                                uint16_t duration, ValueCallback update_cb,
                                bool use_apparent_val) {
    if (minValue >= maxValue) return false;
    if (width < 50) width = 50; 
    if (width > 120) width = 120;
    if (height < 30) height = 30; 
    if (height > 60) height = 60;
    if (duration > 30000) duration = 30000; 
    if (duration < 1000) duration = 1000;

    if (m_popupManager.enqueueProgress(
            width, height, value, minValue, maxValue, title,
            duration, etl::move(update_cb), use_apparent_val)) {
        markDirty();
        return true;
    }
    return false;
}

/**
 * @brief Show an informational popup
 */
bool PixelUI::showPopupInfo(const char* text, const char* title,
                            uint16_t width, uint16_t height,
                            uint16_t duration) {
    if (!text) return false;
    if (m_popupManager.enqueueInfo(width, height, text, title, duration)) {
        markDirty();
        return true;
    }
    return false;
}

/**
 * @brief Show a 4-digit value popup
 */
bool PixelUI::showPopupValue4Digits(int32_t& value, const char* title,
                                    uint16_t width, uint16_t height,
                                    uint16_t duration, ValueCallback update_cb) {
    if (width < 50) width = 50; 
    if (width > 120) width = 120;
    if (height < 30) height = 30; 
    if (height > 60) height = 60;
    if (duration > 30000) duration = 30000; 
    if (duration < 1000) duration = 1000;

    if (m_popupManager.enqueueValue4Digits(
            width, height, value, title, duration, etl::move(update_cb))) {
        markDirty();
        return true;
    }
    return false;
}

/**
 * @brief Trigger a UI fading effect
 */
void PixelUI::markFading() {
    if (!isFading_) { 
        isFading_ = true;
        m_fadeStep = 1;                  
        m_lastFadeTime = getCurrentTime(); 
        markDirty();
    }
}
