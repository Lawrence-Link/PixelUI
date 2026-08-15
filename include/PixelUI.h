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

#pragma once

#include "U8g2lib.h"
#include "config.h"
#include "core/Callbacks.h"
#include "core/IInputHandler.h"
#include "core/ViewManager/ViewManager.h"
#include "ui/IDrawable.h"
#include "core/CommonTypes.h"
#include "core/canvas/Canvas.h"
#include "core/scheduling/UiDeadlineScheduler.h"
#include <etl/atomic.h>
#include <etl/inplace_function.h>

#if PIXELUI_USE_ANIMATION
#include "core/animation/animation.h"
#endif

#if PIXELUI_USE_COROUTINE
#include "core/coroutine/Coroutine.h"
#endif

#if PIXELUI_USE_FOCUS
#include "focus/focus.h"
#endif

#if PIXELUI_USE_POPUP
#include "ui/Popup/PopupManager.h"
#endif

class Coroutine;
class IWidget;
class BlinkState;

/**
 * @class PixelUI
 * @brief The main class for the PixelUI framework.
 *
 * This class serves as the central hub for managing UI state, rendering,
 * animations, and input handling.
 */
class PixelUI
{
public:
    PixelUI(U8G2& u8g2);
    ~PixelUI();

    /**
     * @brief Initializes the UI framework.
     */
    void begin();

    void addCoroutine(Coroutine* coroutine);
    void removeCoroutine(Coroutine* coroutine);
    void clearAllCoroutines() {
#if PIXELUI_USE_COROUTINE
        m_coroutineScheduler.clear();
#endif
    }
    
    size_t getActiveCoroutineCount() {
#if PIXELUI_USE_COROUTINE
        return m_coroutineScheduler.getActiveCount();
#else
        return 0U;
#endif
    }
    size_t getFocusedWidgetCount() const;

    /** @brief Compatibility tick entry; prefer tickFromISR() on embedded hosts. */
    void heartbeat(uint32_t ms);

    /**
     * @brief ISR-safe time input: atomically accumulates elapsed milliseconds.
     *
     * This function never reads or updates UI managers. The configured notify
     * hook runs only when the pending tick total changes from zero to nonzero.
     */
    void tickFromISR(uint32_t elapsedMs);

    /**
     * @brief Consumes pending ticks and updates all managers on the UI task.
     * @return true if elapsed time was consumed.
     */
    bool process();

    /**
     * @brief Runs one UI-task scheduling pass and returns the next delay.
     *
     * The pass consumes pending ISR ticks, updates due managers, renders only
     * when needed, and returns the configured periodic or tickless wake-up.
     */
    uint32_t handler(uint32_t frameIntervalMs);
    
    // animation related functions.
    
    /**
     * @brief Creates and starts a single-value animation.
     * @param value Reference to the value to animate.
     * @param targetValue The final value.
     * @param duration Duration of the animation.
     * @param easing Easing function to use.
     * @param prot Protection status.
     */
    bool animate(int32_t& value, int32_t targetValue, uint32_t duration, EasingType easing = EasingType::LINEAR, PROTECTION prot = PROTECTION::NOT_PROTECTED);
    
    /**
     * @brief Creates and starts a two-value animation.
     * @param x Reference to the x-coordinate.
     * @param y Reference to the y-coordinate.
     * @param targetX The final x-coordinate.
     * @param targetY The final y-coordinate.
     * @param duration Duration of the animation.
     * @param easing Easing function to use.
     * @param prot Protection status.
     */
    bool animate(int32_t& x, int32_t& y, int32_t targetX, int32_t targetY, uint32_t duration, EasingType easing = EasingType::LINEAR, PROTECTION prot = PROTECTION::NOT_PROTECTED);
    
    bool animateCallback(
        int32_t startValue,
        int32_t endValue,
        uint32_t duration,
        EasingType easing,
        ValueCallback callback,
        PROTECTION protection = PROTECTION::NOT_PROTECTED);
    
    /**
     * @brief Clears all unprotected animations.
     */
    void clearUnprotectedAnimations() {
#if PIXELUI_USE_ANIMATION
        m_animationManager.clearUnprotected();
#endif
    }
    
    /**
     * @brief Clears all animations.
     */
    void clearAllAnimations() {
#if PIXELUI_USE_ANIMATION
        m_animationManager.clear();
#endif
    }
    void clearAnimationProtection() {
#if PIXELUI_USE_ANIMATION
        m_animationManager.clearAllProtectionMarks();
#endif
    }
    size_t activeAnimationCount() const {
#if PIXELUI_USE_ANIMATION
        return m_animationManager.activeCount();
#else
        return 0U;
#endif
    }

    uint32_t getCurrentTime() const {
        return _currentTime + pendingTickMs_.load(etl::memory_order_relaxed);
    }

    // setters
    void setRefreshCallback(VoidCallback function) { if (function) m_refresh_callback = function; }
    /**
     * @brief Sets a coalesced wake-up notification for clean-to-dirty changes.
     *
     * The callback should post or signal host-loop work; it must not call
     * renderer() synchronously from inside a PixelUI mutation.
     */
    void setRenderRequestCallback(VoidCallback function);
    /**
     * @brief Installs the ISR-to-UI-task notification hook.
     *
     * Configure this before enabling the timer interrupt and do not replace it
     * while the ISR can run. The function must be an ISR-safe platform API.
     */
    void setTaskNotifyFromISR(IsrTaskNotifyFunction function, void* context = nullptr);
    void setInputCallback(InputCallback callback) { if(callback) inputCallback_ = callback; }
    void clearInputCallback() { inputCallback_ = nullptr; }
    void setContinuousDraw(bool isEnabled) {
        continousMode_ = isEnabled;
        if (isEnabled) markDirty();
    }
    // Backward-compatible spelling retained for existing applications.
    void setContinousDraw(bool isEnabled) { setContinuousDraw(isEnabled); }

    // TBD:
    // void setDebugPrintFunction(void (*func)(const char*)) { if (func) m_func_debug_print = func; }

    // getters
    U8G2& getU8G2() const { return u8g2_; }
    uint16_t getDisplayWidth() const { return displayWidth_; }
    uint16_t getDisplayHeight() const { return displayHeight_; }
    uint16_t getDisplayBufferSize() const { return displayBufferSize_; }
    Canvas& getCanvas() { return canvas_; }
    const Canvas& getCanvas() const { return canvas_; }

    bool scrollCanvasBy(int32_t deltaY);
    bool scrollCanvasTo(int32_t y);
    bool ensureCanvasVisible(int32_t top, int32_t bottom);
    bool animateCanvasTo(int32_t y, uint32_t duration,
                         EasingType easing = EasingType::EASE_OUT_CUBIC,
                         PROTECTION protection = PROTECTION::NOT_PROTECTED);
    ViewManager* getViewManagerPtr() const {
        return const_cast<ViewManager*>(&m_viewManager);
    }

    size_t popupCount() const {
#if PIXELUI_USE_POPUP
        return m_popupManager.getPopupCounts();
#else
        return 0U;
#endif
    }
    void clearPopups() {
#if PIXELUI_USE_POPUP
        if (m_popupManager.getPopupCounts() != 0U) {
            m_popupManager.clearPopups();
            markDirty();
        }
#endif
    }

    // popup related functions

    /**
     * @brief Shows an informational popup.
     * @param text The text content.
     * @param title Optional title.
     * @param width Popup width.
     * @param height Popup height.
     * @param duration Display duration.
     * @return true if the request was accepted.
     */
    bool showPopupInfo(const char* text, const char* title = "", uint16_t width = 80,
                       uint16_t height = 30, uint16_t duration = 3000);

    /**
     * @brief Shows a fixed-width integer editor.
     * @param value Value to edit.
     * @param digitCount Number of editable digits (1..MAX_INT_FIXED_WIDTH).
     * @param title Optional title.
     * @param width Popup width.
     * @param height Popup height.
     * @param duration Display duration.
     * @param update_cb function callback when value changed.
     * @return true if the request was accepted.
     */
    bool showPopupValueDigits(
        int32_t& value,
        uint8_t digitCount,
        const char* title = "", 
        uint16_t width = 80, 
        uint16_t height = 40,
        uint16_t duration = 3000,
        ValueCallback update_cb = nullptr);
    
    /**
     * @brief Shows a progress popup.
     * @param value A reference to the progress value.
     * @param minValue Minimum value.
     * @param maxValue Maximum value.
     * @param title Optional title.
     * @param width Popup width.
     * @param height Popup height.
     * @param duration Display duration.
     * @param update_cb function callback when value changed.
     * @param use_apparent_val Don't display the "%"
     * @return true if the request was accepted.
     */
    bool showPopupProgress(int32_t& value,
        int32_t minValue, 
        int32_t maxValue, 
        const char* title, 
        uint16_t width = 100, 
        uint16_t height = 40, 
        uint16_t duration = 3000,
        ValueCallback update_cb = nullptr, bool use_apparent_val = false);

    /**
     * @brief Marks the display buffer as dirty, forcing a redraw.
     */
    void markDirty();

    /**
     * @brief Returns true when time-driven UI work still needs heartbeat calls.
     *
     * An event loop may stop its periodic UI timer while this returns false and
     * wake it again when an input/data event starts an animation, Popup, fade,
     * coroutine, or continuous drawing.
     */
    bool needsHeartbeat() const;

    /**
     * @brief Returns the host timer delay recommended by the configured policy.
     *
     * With tickless disabled this always returns periodicTickMs. With tickless
     * enabled it returns the earliest manager deadline, zero for immediate
     * work, or WAIT_FOREVER while no time-driven work is active.
     */
    uint32_t nextWakeupMs(uint32_t periodicTickMs) const;
    static constexpr uint32_t WAIT_FOREVER = UINT32_MAX;

    /**
     * @brief Returns true when renderer() has work that may produce a frame.
     */
    bool hasPendingFrame() const;
    /**
     * @brief Marks the UI as fading out.
     */
    void markFading();

    bool addWidgetToFocusManager(IWidget* w);

    void handleInput(InputEvent event);

    void clearFocusManager();
    /**
     * @brief The main rendering function.
     */
    bool renderer();

    friend class ViewManager;
#if PIXELUI_USE_POPUP
    friend class PopupManager;
#endif
#if PIXELUI_USE_FOCUS
    friend class FocusManager;
#endif

protected:
    void setDrawable(IDrawable* drawable) { currentDrawable_ = drawable; }
    bool isFading() const { return isFading_; }

private:
    friend class BlinkState;

    uint32_t calculateNextWakeupMs(uint32_t frameIntervalMs) const;

    U8G2& u8g2_;
    const uint16_t displayWidth_;
    const uint16_t displayHeight_;
    const uint16_t displayBufferSize_;
    Canvas canvas_;
    UiDeadlineScheduler m_deadlineScheduler;

#if PIXELUI_USE_ANIMATION
    AnimationManager m_animationManager;
#endif
#if PIXELUI_USE_POPUP
    PopupManager m_popupManager;
#endif
#if PIXELUI_USE_COROUTINE
    CoroutineScheduler m_coroutineScheduler;
#endif
#if PIXELUI_USE_FOCUS
    FocusManager m_focusManager;
#endif

    uint32_t _currentTime = 0;
    IDrawable* currentDrawable_ = nullptr;
    etl::atomic<uint32_t> pendingTickMs_{0U};

    bool isDirty_ = false;
    bool isFading_ = false;
    bool continousMode_ = false;

    VoidCallback m_refresh_callback = nullptr;
    VoidCallback m_render_request_callback = nullptr;
    IsrTaskNotifyFunction m_taskNotifyFromISR_ = nullptr;
    void* m_taskNotifyContext_ = nullptr;
    InputCallback inputCallback_ = nullptr;

    bool isContinousRefreshEnabled() const { return continousMode_; }
    void (*m_func_debug_print)(const char*) = nullptr;
    uint8_t m_fadeStep = 0;
    uint32_t m_lastFadeTime = 0;

    // Must be last so applications are destroyed before their dependencies.
    ViewManager m_viewManager;
};
