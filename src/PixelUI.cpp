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
#if PIXELUI_USE_POPUP_VALUE_DIGITS
#include "ui/Popup/PopupValueDigits.h"
#endif
#include <inttypes.h>

/**
 * @brief Construct a PixelUI instance with a U8G2 display reference
 * @param u8g2 Reference to the U8G2 display object
 *
 * Initializes core subsystems: ViewManager, AnimationManager, PopupManager, and CoroutineScheduler.
 */
PixelUI::PixelUI(U8G2& u8g2)
    : u8g2_(u8g2)
#if PIXELUI_USE_POPUP
      , m_popupManager(*this)
#endif
#if PIXELUI_USE_COROUTINE
      , m_coroutineScheduler(*this)
#endif
#if PIXELUI_USE_FOCUS
      , m_focusManager(*this)
#endif
      , _currentTime(0)
      , m_viewManager(*this) {
    m_viewManager.attachInputRouter();
}

PixelUI::~PixelUI() = default;

/**
 * @brief Add a coroutine to the scheduler
 * @param coroutine Non-owning pointer to the Coroutine object
 */
void PixelUI::addCoroutine(Coroutine* coroutine) { 
#if PIXELUI_USE_COROUTINE
    m_coroutineScheduler.addCoroutine(coroutine);
#else
    (void)coroutine;
#endif
}

/**
 * @brief Remove a coroutine from the scheduler
 * @param coroutine Non-owning pointer to the Coroutine object
 */
void PixelUI::removeCoroutine(Coroutine* coroutine) { 
#if PIXELUI_USE_COROUTINE
    m_coroutineScheduler.removeCoroutine(coroutine);
#else
    (void)coroutine;
#endif
}

/**
 * @brief Placeholder initialization function
 *
 * Currently empty; kept for API consistency and future expansion.
 */
void PixelUI::begin() { }

/**
 * @brief Compatibility wrapper for hosts that inject ticks outside an ISR.
 */
void PixelUI::Heartbeat(uint32_t ms) {
    pendingTickMs_.fetch_add(ms, etl::memory_order_release);
}

void PixelUI::setTaskNotifyFromISR(IsrTaskNotifyFunction function, void* context) {
    m_taskNotifyContext_ = context;
    m_taskNotifyFromISR_ = function;
}

void PixelUI::tickFromISR(uint32_t elapsedMs) {
    if (elapsedMs == 0U) return;

    const uint32_t previous =
        pendingTickMs_.fetch_add(elapsedMs, etl::memory_order_acq_rel);
    if (previous == 0U && m_taskNotifyFromISR_ != nullptr) {
        m_taskNotifyFromISR_(m_taskNotifyContext_);
    }
}

bool PixelUI::process() {
    const uint32_t elapsedMs =
        pendingTickMs_.exchange(0U, etl::memory_order_acq_rel);
    if (elapsedMs == 0U) return false;

    _currentTime += elapsedMs;

#if PIXELUI_USE_ANIMATION
    const bool animationWasActive = m_animationManager.activeCount() != 0U;
#endif
#if PIXELUI_USE_POPUP
    m_popupManager.updatePopups(_currentTime);
#endif
#if PIXELUI_USE_COROUTINE
    m_coroutineScheduler.update(_currentTime);
#endif
#if PIXELUI_USE_ANIMATION
    m_animationManager.update(_currentTime);
    // Completion removes the animation after writing its final value.
    if (animationWasActive) markDirty();
#endif
    return true;
}

void PixelUI::setRenderRequestCallback(VoidCallback function) {
    m_render_request_callback = etl::move(function);
    if (m_render_request_callback && hasPendingFrame()) {
        m_render_request_callback();
    }
}

void PixelUI::markDirty() {
    bool expected = false;
    if (isDirty_.compare_exchange_strong(
            expected,
            true,
            etl::memory_order_acq_rel,
            etl::memory_order_acquire) &&
        m_render_request_callback) {
        m_render_request_callback();
    }
}

bool PixelUI::animateCallback(
    int32_t startValue,
    int32_t endValue,
    uint32_t duration,
    EasingType easing,
    ValueCallback callback,
    PROTECTION protection) {
#if PIXELUI_USE_ANIMATION
    const bool added = m_animationManager.emplace(
        startValue,
        endValue,
        duration,
        easing,
        etl::move(callback),
        protection,
        getCurrentTime());
    if (added) markDirty();
    return added;
#else
    (void)startValue;
    (void)duration;
    (void)easing;
    (void)protection;
    if (callback) callback(endValue);
    return true;
#endif
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

#if PIXELUI_USE_ANIMATION
    if (m_animationManager.available() < 2U) {
        return false;
    }

    const bool xAdded = animateCallback(
        x, targetX, duration, easing, [&x](int32_t val) { x = val; }, prot);
    const bool yAdded = animateCallback(
        y, targetY, duration, easing, [&y](int32_t val) { y = val; }, prot);
    return xAdded && yAdded;
#else
    (void)duration;
    (void)easing;
    (void)prot;
    x = targetX;
    y = targetY;
    return true;
#endif
}
/**
 * @brief Add a widget to the FocusManager
 * @param w Pointer to the widget to add
 */
bool PixelUI::addWidgetToFocusManager(IWidget* w) {
#if PIXELUI_USE_FOCUS
    return m_focusManager.addWidget(w);
#else
    (void)w;
    return false;
#endif
}
/**
 * @brief Clear all widgets from the FocusManager
 */
void PixelUI::clearFocusManager() {
#if PIXELUI_USE_FOCUS
    m_focusManager.clear();
#endif
}

size_t PixelUI::getFocusedWidgetCount() const {
#if PIXELUI_USE_FOCUS
    return m_focusManager.widgetCount();
#else
    return 0U;
#endif
}
/**
 * @brief Handle input event
 * @param event InputEvent to handle
 *
 * Passes the input event to the FocusManager and the registered input callback.
 */

void PixelUI::handleInput(InputEvent event) {
#if PIXELUI_USE_FOCUS
    if (m_focusManager.handleInput(event)) return;
#endif
    if (inputCallback_) inputCallback_(event);
}

/**
 * @brief Render the current drawable and active popups
 *
 * Handles optional fading effects and calls the refresh callback if set.
 */
bool PixelUI::needsHeartbeat() const {
    if (isFading_ || continousMode_) return true;
#if PIXELUI_USE_ANIMATION
    if (m_animationManager.activeCount() != 0U) return true;
#endif
#if PIXELUI_USE_POPUP
    if (m_popupManager.getPopupCounts() != 0U) return true;
#endif
#if PIXELUI_USE_COROUTINE
    if (m_coroutineScheduler.getActiveCount() != 0U) return true;
#endif
    return false;
}

uint32_t PixelUI::nextWakeupMs(uint32_t periodicTickMs) const {
    if (periodicTickMs == 0U) periodicTickMs = 1U;
#if PIXELUI_ENABLE_TICKLESS
    return needsHeartbeat() ? periodicTickMs : WAIT_FOREVER;
#else
    return periodicTickMs;
#endif
}

bool PixelUI::hasPendingFrame() const {
    return isDirty_.load(etl::memory_order_acquire) || isFading_ || continousMode_;
}

bool PixelUI::renderer() {
    if (m_viewManager.isTransitionCommitInProgress()) return false;

    // Backward compatibility: legacy hosts may still call only renderer(). New
    // hosts should call process() explicitly before renderer() on the UI task.
    process();

    if (isContinousRefreshEnabled()) markDirty();

    if (!isFading_ && !isDirty_.load(etl::memory_order_acquire)) return false;

    if (!isFading_) {
        // Consume the request before drawing so an invalidation raised by draw()
        // remains pending for the next pass instead of being lost.
        isDirty_.store(false, etl::memory_order_release);
        u8g2_.clearBuffer();
        if (currentDrawable_) currentDrawable_->draw();
#if PIXELUI_USE_POPUP
        m_popupManager.drawPopups();
#endif
#if PIXELUI_USE_FOCUS
        m_focusManager.draw();
#endif
        u8g2_.sendBuffer();
        if (m_refresh_callback) m_refresh_callback();
        return true;
    } else {
        if (m_fadeStep == 0) {
            isDirty_.store(false, etl::memory_order_release);
            u8g2_.clearBuffer();
            if (currentDrawable_) currentDrawable_->draw();
#if PIXELUI_USE_POPUP
            m_popupManager.drawPopups();
#endif
            u8g2_.sendBuffer();
            if (m_refresh_callback) m_refresh_callback();
            m_fadeStep = 1;
            m_lastFadeTime = getCurrentTime();
            return true;
        }
        if (m_fadeStep >= 1 && m_fadeStep <= 4) {
            if (getCurrentTime() - m_lastFadeTime < 40) return false;

            isDirty_.store(false, etl::memory_order_release);
            uint8_t *buf_ptr = u8g2_.getBufferPtr();
            const uint16_t buf_len = u8g2_GetBufferSize(u8g2_.getU8g2());
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
            if (m_fadeStep > 4) {
                isFading_ = false;
                m_fadeStep = 0;
                m_viewManager.completePendingEnter();
            }
            return true;
        }
    }
    return false;
}

/**
 * @brief Show a progress popup
 */
bool PixelUI::showPopupProgress(int32_t& value, int32_t minValue, int32_t maxValue,
                                const char* title, uint16_t width, uint16_t height,
                                uint16_t duration, ValueCallback update_cb,
                                bool use_apparent_val) {
#if PIXELUI_USE_POPUP_PROGRESS
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
#else
    (void)value;
    (void)minValue;
    (void)maxValue;
    (void)title;
    (void)width;
    (void)height;
    (void)duration;
    (void)update_cb;
    (void)use_apparent_val;
    return false;
#endif
}

/**
 * @brief Show an informational popup
 */
bool PixelUI::showPopupInfo(const char* text, const char* title,
                            uint16_t width, uint16_t height,
                            uint16_t duration) {
#if PIXELUI_USE_POPUP_INFO
    if (!text) return false;
    if (m_popupManager.enqueueInfo(width, height, text, title, duration)) {
        markDirty();
        return true;
    }
    return false;
#else
    (void)text;
    (void)title;
    (void)width;
    (void)height;
    (void)duration;
    return false;
#endif
}

/**
 * @brief Show a fixed-width value editor popup
 */
bool PixelUI::showPopupValueDigits(int32_t& value, uint8_t digitCount,
                                   const char* title, uint16_t width,
                                   uint16_t height, uint16_t duration,
                                   ValueCallback update_cb) {
#if PIXELUI_USE_POPUP_VALUE_DIGITS
    if (!PopupValueDigits::isValidDigitCount(digitCount)) return false;
    const uint16_t contentWidth = static_cast<uint16_t>(
        digitCount * 12U + (digitCount - 1U) * 2U + 12U);
    if (width < contentWidth) width = contentWidth;
    if (width > 120) width = 120;
    if (height < 40) height = 40;
    if (height > 60) height = 60;
    if (duration > 30000) duration = 30000; 
    if (duration < 1000) duration = 1000;

    if (m_popupManager.enqueueValueDigits(
            width, height, value, digitCount, title, duration, etl::move(update_cb))) {
        markDirty();
        return true;
    }
    return false;
#else
    (void)value;
    (void)digitCount;
    (void)title;
    (void)width;
    (void)height;
    (void)duration;
    (void)update_cb;
    return false;
#endif
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
