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

#include "PixelUI/core/ui/Popup/Popup.h"
#include "PixelUI/pixelui.h"
#include "PixelUI/core/animation/animation.h"

// --- PopupInfo Class Implementation ---

/**
 * @brief Constructs a PopupInfo object, handling text wrapping and initial state setup.
 * @param ui The PixelUI instance to use for drawing and animations.
 * @param width The desired width of the popup box.
 * @param height The desired height of the popup box.
 * @param position The position of the popup on the screen (e.g., CENTER, TOP_LEFT).
 * @param text The message text to display inside the popup.
 * @param title The title text for the popup (optional).
 * @param duration The time in milliseconds the popup should be visible.
 * @param priority The priority level of the popup. Higher priority popups are drawn on top.
 */
PopupInfo::PopupInfo(PixelUI& ui, uint16_t width, uint16_t height, PopupPosition position, 
                     const char* text, const char* title, uint16_t duration, uint8_t priority)
    : m_ui(ui), _width(width), _height(height), _position(position), 
      _title(title), _text(text), _priority(priority), _duration(duration),
      _startTime(0), _currentBoxSize(0), 
      _lineCount(0), _actualHeight(height), _state(State::APPEARING)
{
    // If text is provided, calculate how many lines it will take and adjust the popup's actual height.
    if (_text && _text[0] != '\0') {
        U8G2& u8g2 = m_ui.getU8G2();
        u8g2.setFont(u8g2_font_5x7_tr);  // Set a font for text measurement.
        
        uint16_t textAreaWidth = _width - (TEXT_MARGIN * 2);
        _lineCount = splitTextIntoLines(_text, textAreaWidth);
        
        // Calculate the total height required for the wrapped text.
        uint16_t textHeight = _lineCount * LINE_HEIGHT;
        _actualHeight = textHeight + (TEXT_MARGIN * 2);
        
        // Clamp the actual height to prevent the popup from becoming excessively large or small.
        if (_actualHeight > _height * 2) {
            _actualHeight = _height * 2;
        }
        if (_actualHeight < _height) {
            _actualHeight = _height;
        }
    }
    
    // Use a fixed-point number for smoother animation of the box size.
    _targetBoxSize = _width << 12; 
}

/**
 * @brief Splits a string into multiple lines based on a maximum width.
 * @param text The input string to split.
 * @param maxWidth The maximum allowed width for a single line in pixels.
 * @return The number of lines the text was split into.
 */
uint16_t PopupInfo::splitTextIntoLines(const char* text, uint16_t maxWidth) {
    if (!text || maxWidth == 0) return 0;
    
    U8G2& u8g2 = m_ui.getU8G2();
    uint16_t lineCount = 0;
    const char* currentPos = text;
    
    // Loop through the text until the end or the maximum number of lines is reached.
    while (*currentPos && lineCount < MAX_LINES) {
        const char* lineStart = currentPos;
        const char* lastSpace = nullptr;
        const char* testEnd = currentPos;
        
        // Find the end of the current line by checking width.
        while (*testEnd) {
            if (*testEnd == ' ') {
                lastSpace = testEnd;
            }
            
            // Create a temporary substring to measure its width.
            uint16_t testLength = testEnd - lineStart + 1;
            char tempStr[testLength + 1];
            strncpy(tempStr, lineStart, testLength);
            tempStr[testLength] = '\0';
            
            uint16_t currentWidth = u8g2.getStrWidth(tempStr);
            
            // If the line is too long, backtrack to the last space to avoid cutting a word.
            if (currentWidth > maxWidth) {
                if (lastSpace && lastSpace > lineStart) {
                    testEnd = lastSpace;
                } else if (testEnd > lineStart) {
                    testEnd--;
                }
                break;
            }
            
            testEnd++;
        }
        
        // Store the start pointer and length of the newly found line.
        _textLines[lineCount].start = lineStart;
        _textLines[lineCount].length = testEnd - lineStart;
        lineCount++;
        
        // Move to the beginning of the next line.
        currentPos = testEnd;
        if (*currentPos == ' ') {
            currentPos++; // Skip leading spaces.
        }
    }
    
    return lineCount;
}

/**
 * @brief Updates the popup's state and animation.
 * @param currentTime The current system time in milliseconds.
 * @return true if the popup should remain on screen, false if it should be removed.
 */
bool PopupInfo::update(uint32_t currentTime) {
    if (_startTime == 0) {
        _startTime = currentTime;
        // Start the "appearing" animation by animating the box size.
        m_ui.animate(_currentBoxSize, _targetBoxSize, 300, EasingType::EASE_OUT_CUBIC);
    }
    
    // State machine to control the popup's lifecycle (appearing, showing, closing).
    switch (_state) {
        case State::APPEARING: {
            // Check if the appearance animation is complete.
            if (_currentBoxSize >= _targetBoxSize) {
                _currentBoxSize = _targetBoxSize;
                _state = State::SHOWING;
                _startTime = currentTime; // Reset start time for the showing duration.
            }
            break;
        }
        case State::SHOWING: {
            // Check if the display duration has passed.
            if (currentTime - _startTime >= _duration) {
                _state = State::CLOSING;
                // Start the "closing" animation.
                m_ui.animate(_currentBoxSize, 0, 300, EasingType::EASE_IN_CUBIC);
            }
            break;
        }
        case State::CLOSING: {
            // Check if the closing animation is complete.
            if (_currentBoxSize <= 0) {
                _currentBoxSize = 0;
                return false; // The popup has finished closing and should be removed.
            }
            break;
        }
    }
    
    return true; // The popup is still active.
}

/**
 * @brief Draws the popup box and its contents (title, text) to the display.
 */
void PopupInfo::draw() {
    U8G2& u8g2 = m_ui.getU8G2();
    
    int16_t screenWidth = u8g2.getDisplayWidth();
    int16_t screenHeight = u8g2.getDisplayHeight();
    int16_t centerX = screenWidth / 2;
    int16_t centerY = screenHeight / 2;
    
    // Get the current dimensions from the animated size.
    int16_t currentWidth = _currentBoxSize >> 12;
    if (currentWidth <= 0) return;
    
    // Maintain the aspect ratio during animation.
    int16_t currentHeight = (currentWidth * _actualHeight) / _width;
    
    // Calculate the top-left corner of the popup box.
    int16_t rectX = centerX - currentWidth / 2;
    int16_t rectY = centerY - currentHeight / 2;
    
    // Use a clip window to prevent drawing outside the popup's bounds during animation.
    u8g2.setClipWindow(rectX, rectY, rectX + currentWidth, rectY + currentHeight);
    
    // Draw the popup box with a double-border effect.
    u8g2.setDrawColor(1); // Set draw color to white.
    const int16_t offset = 2;
    u8g2.drawFrame(rectX + offset, rectY + offset, currentWidth, currentHeight);
    u8g2.drawFrame(rectX, rectY, currentWidth, currentHeight);
    
    // Fill the inside of the box with black.
    u8g2.setDrawColor(0); // Set draw color to black.
    u8g2.drawBox(rectX + 1, rectY + 1, currentWidth - 2, currentHeight - 2);
    
    // Draw the wrapped text inside the popup.
    if (_text && _lineCount > 0) {
        u8g2.setDrawColor(1); // Set draw color back to white for text.
        u8g2.setFont(u8g2_font_5x7_tr);
        
        int16_t textAreaHeight = _lineCount * LINE_HEIGHT;
        int16_t textStartY = centerY - textAreaHeight / 2 + LINE_HEIGHT - 2;
        
        for (uint16_t i = 0; i < _lineCount; i++) {
            if (_textLines[i].length > 0) {
                // Get the substring for the current line.
                char lineStr[_textLines[i].length + 1];
                strncpy(lineStr, _textLines[i].start, _textLines[i].length);
                lineStr[_textLines[i].length] = '\0';
                
                // Center the line horizontally.
                int16_t lineWidth = u8g2.getStrWidth(lineStr);
                int16_t lineX = centerX - lineWidth / 2;
                int16_t lineY = textStartY + (i * LINE_HEIGHT);
                
                u8g2.drawStr(lineX, lineY, lineStr);
            }
        }
    }
    
    // Reset the clip window after drawing is complete.
    u8g2.setMaxClipWindow();
    u8g2.setDrawColor(1); // Reset draw color to default.
}

/**
 * @brief Handles input events. Any input immediately triggers the popup to close.
 * @param event The input event received.
 * @return true to indicate the input was handled.
 */
bool PopupInfo::handleInput(InputEvent event) {
    if (_state != State::CLOSING) {
        _state = State::CLOSING;
        m_ui.animate(_currentBoxSize, 0, 300, EasingType::EASE_IN_CUBIC);
    }
    return true;
}

// --- PopupManager Class Implementation ---

/**
 * @brief Adds a new popup to the manager, inserting it based on its priority.
 * @param popup The shared pointer to the popup object to add.
 */
void PopupManager::addPopup(std::shared_ptr<IPopup> popup) {
    if (!popup) return;
    
    // Find the correct insertion point to maintain a sorted list by priority (highest first).
    auto insertPos = _popups.begin();
    for (auto it = _popups.begin(); it != _popups.end(); ++it) {
        if ((*it)->getPriority() < popup->getPriority()) {
            insertPos = it;
            break;
        }
        insertPos = it + 1;
    }
    
    if (_popups.size() < _popups.max_size()) {
        _popups.insert(insertPos, popup);
    }
}

/**
 * @brief Removes a specific popup from the manager.
 * @param popup The shared pointer to the popup object to remove.
 */
void PopupManager::removePopup(std::shared_ptr<IPopup> popup) {
    if (!popup) return;
    
    auto it = std::find(_popups.begin(), _popups.end(), popup);
    if (it != _popups.end()) {
        _popups.erase(it);
    }
}

/**
 * @brief Removes all popups from the manager.
 */
void PopupManager::clearPopups() {
    _popups.clear();
}

/**
 * @brief Draws all active popups in the order of their priority (highest priority drawn last).
 */
void PopupManager::drawPopups() {
    for (const auto& popup : _popups) {
        if (popup) {
            popup->draw();
        }
    }
}

/**
 * @brief Updates the state of all active popups and removes any that have finished.
 * @param currentTime The current system time in milliseconds.
 */
void PopupManager::updatePopups(uint32_t currentTime) {
    if (_popups.empty()) {
        return;
    }

    // Use the erase-remove idiom to efficiently remove finished popups.
    auto writePos = _popups.begin();
    for (auto readPos = _popups.begin(); readPos != _popups.end(); ++readPos) {
        if (*readPos && (*readPos)->update(currentTime)) {
            if (writePos != readPos) {
                *writePos = std::move(*readPos);
            }
            ++writePos;
        }
    }
    _popups.erase(writePos, _popups.end());
}

/**
 * @brief Handles an input event for the highest priority popup.
 * @param event The input event received.
 * @return true if an input was handled, false otherwise.
 */
bool PopupManager::handleTopPopupInput(InputEvent event) {
    if (_popups.empty()) return false;
    
    // Find the popup with the maximum priority.
    auto topPopup = _popups.front();
    for (const auto& popup : _popups) {
        if (popup && popup->getPriority() > topPopup->getPriority()) {
            topPopup = popup;
        }
    }
    
    return topPopup ? topPopup->handleInput(event) : false;
}