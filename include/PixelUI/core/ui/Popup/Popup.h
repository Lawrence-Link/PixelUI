/*
 * Copyright (C) 2025 Lawrence Link
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <functional>
#include <cstdint>
#include "PixelUI/core/CommonTypes.h"
#include "etl/vector.h"
#include "PixelUI/config.h"

// 前向声明,避免循环包含
class PixelUI;

class IPopup{
public:
    IPopup() = default;
    virtual ~IPopup() {}

    virtual bool update(uint32_t currentTime) = 0;
    virtual void draw() = 0;
    virtual bool handleInput(InputEvent event) = 0;
    virtual uint8_t getPriority() const = 0;
    virtual uint16_t getDuration() const = 0;
};

class PopupInfo : public IPopup {
private:
    uint16_t _width, _height;
    uint16_t _actualHeight;  // calculate desired height by font and text
    const char *_title;
    const char *_text;
    PopupPosition _position;
    uint8_t _priority;
    uint16_t _duration;
    uint32_t _startTime;
    int32_t _currentBoxSize;
    int32_t _targetBoxSize;
    PixelUI& m_ui;
    
    // state management
    enum class State {
        APPEARING, // popup
        SHOWING,   // displaying in static
        CLOSING    // shrink
    };
    State _state;

    // text
    static const uint16_t MAX_LINES = 8;  // max number of lines
    static const uint16_t LINE_HEIGHT = 9; // line height in pixel
    static const uint16_t TEXT_MARGIN = 6; // margin of the text to the border
    
    struct TextLine {
        const char* start;
        uint16_t length;
    };
    
    TextLine _textLines[MAX_LINES];
    uint16_t _lineCount;
    
    uint16_t splitTextIntoLines(const char* text, uint16_t maxWidth);
    
public:
    PopupInfo(PixelUI& ui, uint16_t width, uint16_t height, PopupPosition position, 
              const char* text, const char* title = "", uint16_t duration = 3000, uint8_t priority = 0);
    ~PopupInfo() = default;
    
    uint8_t getPriority() const override { return _priority; }
    uint16_t getDuration() const override { return _duration; }
    bool update(uint32_t currentTime) override;
    void draw() override;
    bool handleInput(InputEvent event) override;
};

class PopupManager {
private:
    etl::vector<std::shared_ptr<IPopup>, MAX_POPUP_NUM> _popups;
    PixelUI& m_ui;
public:
    PopupManager(PixelUI& ui) : m_ui(ui) {}
    ~PopupManager() = default;

    void addPopup(std::shared_ptr<IPopup> popup);
    void removePopup(std::shared_ptr<IPopup> popup);
    void clearPopups();
    void drawPopups();
    void updatePopups(uint32_t currentTime);
    bool handleTopPopupInput(InputEvent event);
    size_t getPopupCounts() const { return _popups.size(); }
};