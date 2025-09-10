#include "PixelUI/core/ui/Popup/Popup.h"
#include "PixelUI/core/animation/animation.h"
#include "PixelUI/pixelui.h"

Popup::Popup(IPopupRenderer& renderer, AnimationManager& animManager,
             const std::string& message, PopupType type)
    : renderer_(renderer), animManager_(animManager), message_(message), type_(type),
      isVisible_(false), showTime_(0), duration_(0), boxY_(64), backgroundAlpha_(0) {
}

void Popup::show(uint32_t duration) {
    if (isVisible_) return;
    
    isVisible_ = true;
    duration_ = duration;
    showTime_ = renderer_.getCurrentTime();
    
    animateIn();
    renderer_.markDirty();
}

void Popup::hide() {
    if (!isVisible_) return;
    animateOut();
}

void Popup::animateIn() {
    auto fadeInAnim = std::make_shared<CallbackAnimation>(
        backgroundAlpha_, 0.7f, 200, EasingType::EASE_OUT_QUAD,
        [this](float val) { 
            backgroundAlpha_ = val;
            renderer_.markDirty();
        }
    );
    renderer_.addAnimation(fadeInAnim);
    renderer_.markAnimationProtected(fadeInAnim);
    
    // 弹窗从下方滑入动画
    boxY_ = 64;
    auto slideInAnim = std::make_shared<CallbackAnimation>(
        boxY_, 16.0f, 300, EasingType::EASE_OUT_CUBIC,
        [this](float val) { 
            boxY_ = val;
            renderer_.markDirty();
        }
    );
    renderer_.addAnimation(slideInAnim);
    renderer_.markAnimationProtected(slideInAnim);
}

void Popup::animateOut() {
    // 使用weak_ptr避免循环引用
    std::weak_ptr<Popup> weakSelf = selfWeakPtr_;
    
    auto hideDelayAnim = std::make_shared<CallbackAnimation>(
        0.0f, 1.0f, 250, EasingType::LINEAR,
        [weakSelf, this](float progress) {
            auto self = weakSelf.lock();
            if (!self) return;  // 对象已被销毁，直接返回
            
            if (progress >= 1.0f) {
                isVisible_ = false;
                if (onCloseCallback_) {
                    onCloseCallback_();
                }
                renderer_.markDirty();
            }
        }
    );
    renderer_.addAnimation(hideDelayAnim);
    renderer_.markAnimationProtected(hideDelayAnim);
}

void Popup::update(uint32_t currentTime) {
    if (!isVisible_) return;
    
    // 检查是否需要自动关闭
    if (duration_ > 0 && (currentTime - showTime_) >= duration_) {
        hide();
    }
}

void Popup::draw() {
    if (!isVisible_) return;
    
    // 绘制半透明背景
    if (backgroundAlpha_ > 0.01f) {  // 避免绘制完全透明的背景
        renderer_.setDrawColor(1);
        // 使用简单的点阵模式模拟半透明效果
        int alphaLevel = static_cast<int>(backgroundAlpha_ * 4);  // 0-2的透明度级别
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 128; x++) {
                bool shouldDraw = false;
                switch (alphaLevel) {
                    case 1: shouldDraw = (x + y) % 4 == 0; break;
                    case 2: shouldDraw = (x + y) % 3 == 0; break;
                    case 3: shouldDraw = (x + y) % 2 == 0; break;
                    default: shouldDraw = alphaLevel >= 4; break;
                }
                if (shouldDraw) {
                    renderer_.drawPixel(x, y);
                }
            }
        }
    }
    
    drawBox();
    drawMessage();
    if (type_ == PopupType::CONFIRM) {
        drawButtons();
    }
}

void Popup::drawBox() {
    int boxWidth = 100;
    int boxHeight = 32;
    int boxX = (128 - boxWidth) / 2;
    int currentBoxY = static_cast<int>(boxY_);
    
    // 确保弹窗在屏幕范围内
    if (currentBoxY < 0) currentBoxY = 0;
    if (currentBoxY > 64 - boxHeight) currentBoxY = 64 - boxHeight;
    
    // 绘制弹窗背景
    renderer_.setDrawColor(1);
    renderer_.drawRBox(boxX, currentBoxY, boxWidth, boxHeight, 3);
    
    // 绘制弹窗边框
    renderer_.setDrawColor(0);
    renderer_.drawRFrame(boxX + 1, currentBoxY + 1, boxWidth - 2, boxHeight - 2, 2);
    
    // 根据类型绘制图标
    renderer_.setDrawColor(0);
    switch (type_) {
        case PopupType::INFO:
            renderer_.drawStr(boxX + 5, currentBoxY + 12, "i");
            break;
        case PopupType::WARNING:
            renderer_.drawStr(boxX + 5, currentBoxY + 12, "!");
            break;
        case PopupType::ERROR:
            renderer_.drawStr(boxX + 5, currentBoxY + 12, "X");
            break;
        case PopupType::CONFIRM:
            renderer_.drawStr(boxX + 5, currentBoxY + 12, "?");
            break;
    }
}

void Popup::drawMessage() {
    int boxX = (128 - 100) / 2;
    int currentBoxY = static_cast<int>(boxY_);
    
    // 确保文字在屏幕范围内
    if (currentBoxY < 0) currentBoxY = 0;
    if (currentBoxY > 64 - 32) currentBoxY = 64 - 32;
    
    renderer_.setDrawColor(0);
    
    // 简单的文字绘制，支持自动换行
    const char* text = message_.c_str();
    int maxCharsPerLine = 12;  // 根据字体大小调整
    int lineHeight = 10;
    int startY = currentBoxY + 12;
    
    std::string currentLine;
    int lineCount = 0;
    const int maxLines = 2;  // 最多显示2行
    
    for (size_t i = 0; i < message_.length() && lineCount < maxLines; ++i) {
        char currentChar = text[i];
        
        // 处理换行符或达到行宽限制
        if (currentChar == '\n' || currentLine.length() >= maxCharsPerLine) {
            if (!currentLine.empty()) {
                renderer_.drawStr(boxX + 18, startY + lineCount * lineHeight, currentLine.c_str());
                currentLine.clear();
                lineCount++;
            }
            
            // 如果是字符而不是换行符，将当前字符加入下一行
            if (currentChar != '\n' && lineCount < maxLines) {
                currentLine += currentChar;
            }
        } else {
            currentLine += currentChar;
        }
    }
    
    // 绘制最后一行
    if (!currentLine.empty() && lineCount < maxLines) {
        renderer_.drawStr(boxX + 18, startY + lineCount * lineHeight, currentLine.c_str());
    }
    
    // 如果文字被截断，显示省略号
    if (message_.length() > maxCharsPerLine * maxLines) {
        renderer_.drawStr(boxX + 90, startY + (maxLines - 1) * lineHeight, "...");
    }
}

void Popup::drawButtons() {
    if (type_ != PopupType::CONFIRM) return;
    
    int boxX = (128 - 100) / 2;
    int currentBoxY = static_cast<int>(boxY_);
    
    // 确保按钮在屏幕范围内
    if (currentBoxY < 0) currentBoxY = 0;
    if (currentBoxY > 64 - 32) currentBoxY = 64 - 32;
    
    renderer_.setDrawColor(0);
    
    // 绘制OK和Cancel按钮
    renderer_.drawStr(boxX + 20, currentBoxY + 28, "OK");
    renderer_.drawStr(boxX + 65, currentBoxY + 28, "Cancel");
    
    // 可以添加按钮边框或高亮效果
    // renderer_.drawRFrame(boxX + 15, currentBoxY + 22, 25, 10, 1);  // OK按钮框
    // renderer_.drawRFrame(boxX + 60, currentBoxY + 22, 35, 10, 1);  // Cancel按钮框
}

// 在Popup.cpp末尾添加这个方法实现

void Popup::handleConfirm(bool confirmed) {
    if (!isVisible_) return;
    
    switch (type_) {
        case PopupType::CONFIRM:
            // 确认类型popup的处理
            if (onConfirmCallback_) {
                onConfirmCallback_(confirmed);
            }
            hide();
            break;
            
        case PopupType::INFO:
        case PopupType::WARNING:
        case PopupType::ERROR:
            // 其他类型的popup，任何确认操作都直接关闭
            hide();
            break;
            
        default:
            hide();
            break;
    }
    
    renderer_.markDirty();
    
    std::cout << "[DEBUG] Popup (" << static_cast<int>(type_) 
              << "): handleConfirm called with value: " 
              << (confirmed ? "true" : "false") << std::endl;
}