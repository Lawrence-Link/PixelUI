#pragma once
#include "IPopupRenderer.h"
#include <string>
#include <memory>
#include <functional>
#include <cstdint>
#include "PixelUI/core/CommonTypes.h"
// 前向声明
class AnimationManager;

class Popup {
public:
    Popup(IPopupRenderer& renderer, AnimationManager& animManager, 
          const std::string& message, PopupType type = PopupType::INFO);
    ~Popup() = default;

    void show(uint32_t duration = 0);
    void hide();
    void draw();
    void update(uint32_t currentTime);

    // 回调设置
    void setOnCloseCallback(std::function<void()> callback) { onCloseCallback_ = callback; }
    void setOnConfirmCallback(std::function<void(bool)> callback) { onConfirmCallback_ = callback; }

    // 状态查询
    bool isVisible() const { return isVisible_; }
    PopupType getType() const { return type_; }
    uint32_t getShowTime() const { return showTime_; }

    // 输入处理 - 之前遗漏的方法
    void handleConfirm(bool confirmed);
    void setSelfWeakPtr(std::weak_ptr<Popup> weakPtr) { selfWeakPtr_ = weakPtr; }
    
private:
    IPopupRenderer& renderer_;
    AnimationManager& animManager_;
    std::string message_;
    PopupType type_;
    bool isVisible_;
    uint32_t showTime_;
    uint32_t duration_;
    std::weak_ptr<Popup> selfWeakPtr_;

    float boxY_;
    float backgroundAlpha_;
    
    std::function<void()> onCloseCallback_;
    std::function<void(bool)> onConfirmCallback_;
    
    void animateIn();
    void animateOut();
    void drawBox();
    void drawMessage();
    void drawButtons();
};