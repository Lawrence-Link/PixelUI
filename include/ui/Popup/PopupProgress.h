/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupBase.h"
#include <stddef.h>
#include <etl/inplace_function.h>
#include "config.h"

/**
 * @class PopupProgress
 * @brief A popup displaying a progress bar.
 */
class PopupProgress : public PopupBase {
private:
    int32_t& _value;
    int32_t _minValue, _maxValue;
    const char* _title;
    
    void formatValue(char* buffer, size_t bufferSize) const;
    void formatValueAsPercentage(char* buffer, size_t bufferSize) const;
    
    etl::inplace_function<void(int32_t value), CALLBACK_STORAGE_SIZE> m_cb;
    bool use_apparent_val;
public:
    PopupProgress(PixelUI& ui, uint16_t width, uint16_t height, 
                  int32_t& value, int32_t minValue, int32_t maxValue,
                  const char* title = "", uint16_t duration = 3000,
                  etl::inplace_function<void(int32_t value), CALLBACK_STORAGE_SIZE> cb_function = nullptr,
                  bool UseApparentVal = false);
    ~PopupProgress() = default;

    void drawContent(const PopupContentBounds& bounds) override;
    bool handleContentInput(InputEvent event) override;
};
