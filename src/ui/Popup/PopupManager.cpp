/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ui/Popup/PopupManager.h"
#include <algorithm>

/**
 * @brief Add a popup to the manager
 * @param popup Shared pointer to an IPopup
 *
 * Maintains the _popups container in ascending priority order
 * (low priority first, high priority last). If the container is full,
 * removes the lowest priority popup to make room for the new one.
 */
void PopupManager::addPopup(std::shared_ptr<IPopup> popup) {
    // std::lock_guard<std::mutex> lock(mutex_);
    if (!popup) return;

    if (_popups.size() >= _popups.max_size()) {
        if (!_popups.empty()) {
            auto minPriorityIt = _popups.begin();
            for (auto it = _popups.begin(); it != _popups.end(); ++it) {
                if ((*it)->getPriority() < (*minPriorityIt)->getPriority()) {
                    minPriorityIt = it;
                }
            }
            _popups.erase(minPriorityIt);
        }
    }

    auto insertPos = _popups.begin();
    for (; insertPos != _popups.end(); ++insertPos) {
        if ((*insertPos)->getPriority() > popup->getPriority()) {
            break;
        }
    }
    _popups.insert(insertPos, popup);
}

/**
 * @brief Remove a specific popup from the manager
 * @param popup Shared pointer to the popup to remove
 *
 * Finds the popup in the container and erases it.
 */
void PopupManager::removePopup(std::shared_ptr<IPopup> popup) {
    // std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(_popups.begin(), _popups.end(), popup);
    if (it != _popups.end()) {
        _popups.erase(it);
    }
}

/**
 * @brief Clear all popups from the manager
 */
void PopupManager::clearPopups() {
    // std::lock_guard<std::mutex> lock(mutex_);
    _popups.clear();
}

/**
 * @brief Draw all popups in order
 *
 * Lower priority popups are drawn first, higher priority popups
 * are drawn on top.
 */
void PopupManager::drawPopups() {
    // std::lock_guard<std::mutex> lock(mutex_);
    for (auto& popup : _popups) {
        popup->draw();
    }
}

/**
 * @brief Update all popups with the current time
 * @param currentTime Current system time in milliseconds
 *
 * Calls each popup's update() method. Removes popups that
 * return false (finished animation or expired).
 */
void PopupManager::updatePopups(uint32_t currentTime) {
    // std::lock_guard<std::mutex> lock(mutex_);
    auto it = _popups.begin();
    while (it != _popups.end()) {
        if (!(*it)->update(currentTime)) {
            it = _popups.erase(it);  // remove finished popup
        } else {
            ++it;
        }
    }
}

/**
 * @brief Handle input for the topmost popup first
 * @param event Input event
 * @return true if any popup consumed the event, false otherwise
 *
 * Iterates from highest priority to lowest, giving topmost popup
 * the first chance to consume the input.
 */
bool PopupManager::handleTopPopupInput(InputEvent event) {
    // std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = _popups.rbegin(); it != _popups.rend(); ++it) {
        if ((*it)->handleInput(event)) {
            return true; 
        }
    }
    return false;
}
