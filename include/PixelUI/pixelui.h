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

#pragma once

#include "U8g2lib.h"
#include "PixelUI/core/animation/animation.h"
#include "PixelUI/core/ui/IDrawable.h"
#include "PixelUI/core/CommonTypes.h"



class IInputHandler{
public:
    virtual bool handleInput(InputEvent event) = 0;
    virtual ~IInputHandler() = default;
};

using InputCallback = std::function<bool(InputEvent)>;

typedef void (*DelayFunction)(uint32_t);

class ViewManager;
class PopupManager;

class PixelUI
{
public:
    PixelUI(U8G2& u8g2);
    ~PixelUI() = default;

    void begin();

    void Heartbeat(uint32_t ms);
    
    // animation related functions.
    void animate(int32_t& value, int32_t targetValue, uint32_t duration, EasingType easing = EasingType::LINEAR, PROTECTION prot = PROTECTION::NOT_PROTECTED);
    void animate(int32_t& x, int32_t& y, int32_t targetX, int32_t targetY, uint32_t duration, EasingType easing = EasingType::LINEAR, PROTECTION prot = PROTECTION::NOT_PROTECTED);
    void addAnimation(std::shared_ptr<Animation> animation);
    void markAnimationProtected(std::shared_ptr<Animation> animation) { m_animationManagerPtr->markProtected(animation); }
    void clearUnprotectedAnimations() { m_animationManagerPtr->clearUnprotected(); }
    void clearAllProtectionMarks() { m_animationManagerPtr->clearAllProtectionMarks(); }
    void clearAllAnimations() { m_animationManagerPtr->clear(); }

    uint32_t getCurrentTime() const { return _currentTime; }

    // setters
    void setDrawable(std::shared_ptr<IDrawable> drawable) { currentDrawable_ = drawable; }
    void setRefreshCallback(std::function <void()> function) { if (function) m_refresh_callback = function; }
    void setInputCallback(InputCallback callback) { if(callback) inputCallback_ = callback; }
    void setContinousDraw(bool isEnabled) { continousMode_ = isEnabled; };
    void setDelayFunction(DelayFunction func) {if (func) m_func_delay = func; }
    void setDebugPrintFunction(void (*func)(const char*)) { if (func) m_func_debug_print = func; }

    #ifdef USE_DEBUG_OUPUT
        void debugPrint(const char* msg);
    #endif
    
    // getters
    U8G2& getU8G2() const { return u8g2_; }
    std::shared_ptr<AnimationManager> getAnimationManPtr() { return m_animationManagerPtr; }
    std::shared_ptr<PopupManager> getPopupManagerPtr() { return m_popupManagerPtr; }

    bool isDirty() const { return isDirty_; }
    bool isFading() const { return isFading_; }
    bool isPointerValid(const void* ptr) const { return ptr != nullptr; }

    bool isContinousRefreshEnabled() const { return continousMode_; }
    uint32_t getActiveAnimationCount() const { return m_animationManagerPtr->activeCount(); }

    std::shared_ptr<IDrawable> getDrawable() const { return currentDrawable_; }

    std::shared_ptr<ViewManager> getViewManagerPtr() const { return m_viewManagerPtr; }

    // popup related functions

    void showPopupInfo(const char* text, const char* title = "", uint16_t width = 80, uint16_t height = 30, PopupPosition position = PopupPosition::CENTER, uint16_t duration = 3000, uint8_t priority = 0);

    void markDirty() { isDirty_ = true; }
    void markFading() { isFading_ = true; }

    bool handleInput(InputEvent event) {
        if (inputCallback_) return inputCallback_(event);
        return false;
    }
    void renderer();

protected:
private:
    U8G2& u8g2_;

    std::shared_ptr<AnimationManager> m_animationManagerPtr;
    std::shared_ptr<ViewManager> m_viewManagerPtr;
    std::shared_ptr<PopupManager> m_popupManagerPtr;

    uint32_t _currentTime = 0;
    std::shared_ptr<IDrawable> currentDrawable_;

    bool isDirty_ = false;
    bool isFading_ = false;
    bool continousMode_ = false;

    std::function<void()> m_refresh_callback = nullptr;
    DelayFunction m_func_delay = nullptr;
    InputCallback inputCallback_ = nullptr;
    
    #ifdef USE_DEBUG_OUPUT
        void (*m_func_debug_print)(const char*) = nullptr;
    #endif
};