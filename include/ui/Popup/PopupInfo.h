/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupBase.h"
#include "U8g2lib.h"
#include "config.h"

/**
 * @class PopupInfo
 * @brief A popup displaying informational text.
 */
class PopupInfo : public PopupBase {
private:
    uint16_t _actualHeight;
    const char *_title;
    const char *_text;
    
    static const uint16_t MAX_LINES = 6;
    uint16_t LINE_HEIGHT = 12;
    static const uint16_t TEXT_MARGIN = 4;
    
    struct TextLine {
        const char* start;
        uint16_t length;
    };
    
    TextLine _textLines[MAX_LINES];
    uint16_t _lineCount;

    uint16_t splitTextIntoLines(const char* text, uint16_t maxWidth);
    const uint8_t * m_font = nullptr;
public:
    PopupInfo(PixelUI& ui, uint16_t width, uint16_t height, 
              const char* text, const char* title = "", uint16_t duration = 3000,
              const uint8_t* font = PIXELUI_FONT_TEXT);
    ~PopupInfo() = default;
    
    void drawContent(const PopupContentBounds& bounds) override;
};
