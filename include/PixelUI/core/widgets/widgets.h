#pragma once

#include "PixelUI/pixelui.h"

class Widget{
public:
    virtual ~Widget() = default;
    virtual void draw() = 0;
    virtual void handleEvent(int event) {};

    virtual void onLoad() = 0;
    virtual void onOffload () = 0;

    bool isSelectable() const { return selectable; }
private:
    bool selectable = false;
};