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
#include "core/animation/animation.h"
#include "core/Callbacks.h"
#include "core/coroutine/Coroutine.h"
#include "ui/IDrawable.h"
#include "ui/Popup/PopupManager.h"
#include "core/CommonTypes.h"
#include "config.h"
#include <etl/atomic.h>
#include <etl/inplace_function.h>
#include <etl/memory.h>

/**
 * @class IInputHandler
 * @brief An interface for handling input events.
 */
class IInputHandler{
public:
    virtual bool handleInput(InputEvent event) = 0;
    virtual ~IInputHandler() = default;
};

class ViewManager;
class CoroutineScheduler;
class FocusManager;
class IWidget;

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
    void clearAllCoroutines() { m_coroutineSchedulerPtr->clear(); }
    
    size_t getActiveCoroutineCount() { return m_coroutineSchedulerPtr->getActiveCount(); }
    size_t getFocusedWidgetCount() const;

    /**
     * @brief Main update loop to be called periodically.
     * @param ms Time elapsed since the last call.
     */
    void Heartbeat(uint32_t ms);
    
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
    void clearUnprotectedAnimations() { m_animationManager.clearUnprotected(); }
    
    /**
     * @brief Clears all animations.
     */
    void clearAllAnimations() { m_animationManager.clear(); }
    void clearAnimationProtection() { m_animationManager.clearAllProtectionMarks(); }
    size_t activeAnimationCount() const { return m_animationManager.activeCount(); }

    uint32_t getCurrentTime() const { return _currentTime; }

    // setters
    void setRefreshCallback(VoidCallback function) { if (function) m_refresh_callback = function; }
    void setInputCallback(InputCallback callback) { if(callback) inputCallback_ = callback; }
    void clearInputCallback() { inputCallback_ = nullptr; }
    void setContinousDraw(bool isEnabled) { continousMode_ = isEnabled; };
    void setDelayFunction(DelayFunction func) {if (func) m_func_delay = func; }
    
    // TBD:
    // void setDebugPrintFunction(void (*func)(const char*)) { if (func) m_func_debug_print = func; }

    // getters
    U8G2& getU8G2() const { return u8g2_; }
    ViewManager* getViewManagerPtr() const { return m_viewManagerPtr.get(); }

    size_t popupCount() const { return m_popupManager.getPopupCounts(); }
    void clearPopups() { m_popupManager.clearPopups(); }

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
     * @brief Shows an popup contain 4 digits.
     * @param text The text content.
     * @param title Optional title.
     * @param width Popup width.
     * @param height Popup height.
     * @param duration Display duration.
     * @param update_cb function callback when value changed.
     * @return true if the request was accepted.
     */
    bool showPopupValue4Digits(
        int32_t& value,
        const char* title = "", 
        uint16_t width = 80, 
        uint16_t height = 30, 
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
    void markDirty() { isDirty_ = true; }
    /**
     * @brief Marks the UI as fading out.
     */
    void markFading();

    void addWidgetToFocusManager(IWidget* w);

    void handleInput(InputEvent event);

    void clearFocusManager();
    /**
     * @brief The main rendering function.
     */
    void renderer();

    friend class ViewManager;
    friend class PopupManager;
    friend class FocusManager;

protected:
    void setDrawable(IDrawable* drawable) { currentDrawable_ = drawable; }
    bool isFading() const { return isFading_; }

private:
    U8G2& u8g2_;

    AnimationManager m_animationManager;
    PopupManager m_popupManager;
    etl::unique_ptr<ViewManager> m_viewManagerPtr;
    etl::unique_ptr<CoroutineScheduler> m_coroutineSchedulerPtr;
    etl::unique_ptr<FocusManager> m_focusManagerPtr;

    uint32_t _currentTime = 0;
    IDrawable* currentDrawable_ = nullptr;
    etl::atomic<bool> update_symbol_{false};

    bool isDirty_ = false;
    bool isFading_ = false;
    bool continousMode_ = false;

    VoidCallback m_refresh_callback = nullptr;
    DelayFunction m_func_delay = nullptr;
    InputCallback inputCallback_ = nullptr;

    bool isContinousRefreshEnabled() const { return continousMode_; }
    void (*m_func_debug_print)(const char*) = nullptr;
    uint8_t m_fadeStep = 0;
    uint32_t m_lastFadeTime = 0;
};
