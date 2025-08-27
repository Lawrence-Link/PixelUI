#pragma once

#include <iostream>
#include "PixelUI/core/animation/anime.h"
#include <u8g2_wrapper.h>
#include "PixelUI/core/ui/Popup/IPopupRenderer.h"
#include "PixelUI/core/ui/Popup/PopupManager.h"
#include "PixelUI/core/ui/IDrawable.h"

enum class InputEvent {
    UP, DOWN, LEFT, RIGHT, SELECT, BACK
};

enum class PROTECTION {
    NOT_PROTECTED,
    PROTECTED
};

class IInputHandler{
public:
    virtual bool handleInput(InputEvent event) = 0;
    virtual ~IInputHandler() = default;
};

using InputCallback = std::function<bool(InputEvent)>;

class PixelUI : public IPopupRenderer
{
public:
    PixelUI(U8G2Wrapper& u8g2) : u8g2_(u8g2), _currentTime(0) {
        popupManager_ = std::make_unique<PopupManager>(*this, _animationManager);
    }
    ~PixelUI() = default;

    void begin();
    void Heartbeat(uint32_t ms);
    
    void animate(float& value, float targetValue, uint32_t duration, EasingType easing = EasingType::LINEAR, PROTECTION prot = PROTECTION::NOT_PROTECTED);
    void animate(float& x, float& y, float targetX, float targetY, uint32_t duration, EasingType easing = EasingType::LINEAR, PROTECTION prot = PROTECTION::NOT_PROTECTED);

    void addAnimation(std::shared_ptr<Animation> animation);

    // IPopupRenderer接口实现
    uint32_t getCurrentTime() const override { return _currentTime; }

    void markAnimationProtected(std::shared_ptr<Animation> animation) override {
        _animationManager.markProtected(animation);
    }
    
    // 绘制接口实现
    void setDrawColor(int color) override { u8g2_.setDrawColor(color); }
    void drawRBox(int x, int y, int w, int h, int r) override { u8g2_.drawRBox(x, y, w, h, r); }
    void drawRFrame(int x, int y, int w, int h, int r) override { u8g2_.drawRFrame(x, y, w, h, r); }
    void drawStr(int x, int y, const char* str) override { u8g2_.drawStr(x, y, str); }
    void drawPixel(int x, int y) override { u8g2_.drawPixel(x, y); }
    void setFont(const uint8_t* font) override { u8g2_.setFont(font); }

    // setters
    void setDrawable(std::shared_ptr<IDrawable> drawable) { currentDrawable_ = drawable; }
    void setEmuRefreshFunc(std::function <void()> function) { emu_refresh_func_ = function; }
    void setInputCallback(InputCallback callback) { inputCallback_ = callback; }
    void setContinousDraw(bool isEnabled) { continousMode_ = isEnabled; };

    // getters
    U8G2Wrapper& getU8G2() const { return u8g2_; }
    AnimationManager& getAnimationMan() { return _animationManager; }
    PopupManager& getPopupManager() { return *popupManager_; }
    bool isDirty() const { return isDirty_; }
    bool isPointerValid(const void* ptr) const { return ptr != nullptr; }
    bool isContinousRefreshEnabled() const { return continousMode_; }
    uint32_t getActiveAnimationCount() const { return _animationManager.activeCount(); }
    std::function <void()> getEmuRefreshFunction() {return emu_refresh_func_; } // TBD
    std::shared_ptr<IDrawable> getDrawable() const { return currentDrawable_; }

    void showInfoPopup(const std::string& message, uint32_t duration = 2000) {
        popupManager_->showInfo(message, duration);
    }
    
    void showWarningPopup(const std::string& message, uint32_t duration = 3000) {
        popupManager_->showWarning(message, duration);
    }
    
    void showErrorPopup(const std::string& message, uint32_t duration = 4000) {
        popupManager_->showError(message, duration);
    }
    
    void showConfirmPopup(const std::string& message, std::function<void(bool)> callback) {
        popupManager_->showConfirm(message, callback);
    }

    void markDirty() { isDirty_ = true; }
    void markFading() { isFading_ = true; }

    bool handleInput(InputEvent event) {
        if (inputCallback_) return inputCallback_(event);
        return false;
    }
    void renderer();
protected:
private:
    U8G2Wrapper& u8g2_;
    AnimationManager _animationManager;
    uint32_t _currentTime;
    std::shared_ptr<IDrawable> currentDrawable_;
    std::unique_ptr<PopupManager> popupManager_;

    bool isDirty_ = false;
    bool isFading_ = false;
    bool continousMode_ = false;

    std::function<void()> emu_refresh_func_;

    InputCallback inputCallback_;

    mutable size_t _totalAnimationsCreated = 0;
    mutable size_t _animationUpdateCalls = 0;
};