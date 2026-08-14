#pragma once

#include "core/CommonTypes.h"

class IInputHandler {
public:
    virtual bool handleInput(InputEvent event) = 0;
    virtual ~IInputHandler() = default;
};
