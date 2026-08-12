/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupBase.h"
#include <stddef.h>
#include <etl/inplace_function.h>
#include "config.h"
#include "widgets/num_scroll/num_scroll.h"
#include "focus/focus.h"
/**
 * @class PopupProgress
 * @brief A popup displaying a value window
 */
class PopupValue4Digits : public PopupBase {
private:
    int32_t& _value;
    const char* _title;
    NumScroll num_thousands, num_hundreds, num_tens, num_ones;
    FocusManager m_focusMan;
    etl::inplace_function<void(int32_t value), CALLBACK_STORAGE_SIZE> m_cb;

public:
    PopupValue4Digits(PixelUI& ui, uint16_t width, uint16_t height, 
                  int32_t& value,
                  const char* title = "", uint16_t duration = 3000,
                  etl::inplace_function<void(int32_t value), CALLBACK_STORAGE_SIZE> cb_function = nullptr);
    ~PopupValue4Digits();

    void drawContent(const PopupContentBounds& bounds) override;
    bool handleContentInput(InputEvent event) override;
};
