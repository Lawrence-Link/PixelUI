#pragma once
#include <cstdint>
#include <functional>
#include <memory>

// 前向声明
class Animation;
enum class EasingType;
enum class PROTECTION;

// 渲染接口 - 避免直接依赖PixelUI
class IPopupRenderer {
public:
    virtual ~IPopupRenderer() = default;
    virtual void markDirty() = 0;
    virtual uint32_t getCurrentTime() const = 0;
    virtual void addAnimation(std::shared_ptr<Animation> animation) = 0;
    virtual void markAnimationProtected(std::shared_ptr<Animation> animation) = 0;
    
    // 绘制接口
    virtual void setDrawColor(int color) = 0;
    virtual void drawRBox(int x, int y, int w, int h, int r) = 0;
    virtual void drawRFrame(int x, int y, int w, int h, int r) = 0;
    virtual void drawStr(int x, int y, const char* str) = 0;
    virtual void drawPixel(int x, int y) = 0;
    virtual void setFont(const uint8_t* font) = 0;
};