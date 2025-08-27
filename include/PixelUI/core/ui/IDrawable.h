#pragma once

#include <cstdint>

class IDrawable {
public:
    virtual void draw() = 0;
    virtual ~IDrawable() = default; 
    virtual void update(uint32_t currentTime) {}
};