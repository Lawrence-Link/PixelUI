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

#include "focus/focus.h"
#include "PixelUI.h"

void FocusManager::enterIdle(bool clearSelection) {
    m_state = State::IDLE;
    if (clearSelection) {
        index = -1;
    }
}

void FocusManager::beginFocusAnimation(int nextIndex) {
    index = nextIndex;
    m_state = State::ANIMATING;
    last_focus_change_time = m_ui.getCurrentTime();
    m_target_focus_box = m_Widgets[index]->getFocusBox();

    m_ui.animate(m_current_focus_box.x, m_target_focus_box.x, 100, EasingType::EASE_OUT_QUAD);
    m_ui.animate(m_current_focus_box.y, m_target_focus_box.y, 100, EasingType::EASE_OUT_QUAD);
    m_ui.animate(m_current_focus_box.w, m_target_focus_box.w, 100, EasingType::EASE_OUT_QUAD);
    m_ui.animate(m_current_focus_box.h, m_target_focus_box.h, 100, EasingType::EASE_OUT_QUAD);
}

void FocusManager::enterFocused(bool synchronizeBox) {
    m_state = State::FOCUSED;
    if (synchronizeBox && index >= 0 && index < static_cast<int>(m_Widgets.size())) {
        last_focus_change_time = m_ui.getCurrentTime();
        m_current_focus_box = m_Widgets[index]->getFocusBox();
        m_target_focus_box = m_current_focus_box;
    }
}

void FocusManager::beginShrinkAnimation() {
    m_state = State::ANIMATING_SHRINK;

    const int32_t center_x = m_current_focus_box.x + m_current_focus_box.w / 2;
    const int32_t center_y = m_current_focus_box.y + m_current_focus_box.h / 2;
    m_ui.animate(m_current_focus_box.w, 0, 100, EasingType::EASE_IN_QUAD);
    m_ui.animate(m_current_focus_box.h, 0, 100, EasingType::EASE_IN_QUAD);
    m_ui.animate(m_current_focus_box.x, center_x, 100, EasingType::EASE_IN_QUAD);
    m_ui.animate(m_current_focus_box.y, center_y, 100, EasingType::EASE_IN_QUAD);
}

void FocusManager::resetState() {
    if (m_activeWidget) {
        m_activeWidget->onDeactivate();
    }
    m_activeWidget = nullptr;
    m_current_focus_box = {0, 64, 0, 0};
    m_target_focus_box = m_current_focus_box;
    enterIdle(true);
}

/**
 * @brief Clear the currently active widget, if any, and restore focus state.
 *
 * Called when a widget times out or loses activation.
 */
void FocusManager::clearActiveWidget() {
    if (m_activeWidget) {
        m_activeWidget->onDeactivate();
        m_activeWidget = nullptr;
        
        // Restore focus display state
        if (index >= 0 && index < (int)m_Widgets.size()) {
            enterFocused(true);
        } else {
            enterIdle(true);
        }
    }
}
/**
 * @brief Handle input events for focus management.
 * @param event The input event to handle.
 * @return true if the event was handled by an active widget, false otherwise.
 */
bool FocusManager::handleInput(InputEvent event) {
    // Check if an interactive widget (like the expanded histogram) has taken over input control
    IWidget* activeWidget = getActiveWidget();
    if (activeWidget) {
        // Pass the event to the active widget
        if (activeWidget->handleEvent(event)) {
            // If the widget returns true, it signifies the operation is complete (e.g., expanded view closed)
            // and control should be returned to the FocusManager.
            clearActiveWidget();
        }
        return true; // Event was handled by an active widget
    }

    // No widget has taken over input; proceed with standard focus management and app control
    if (!m_Widgets.empty()) {
        if (event == FOCUS_MANAGER_NAVI_NEXT) {
            moveNext(); // Move focus to the next widget
            return true;
        } else if (event == FOCUS_MANAGER_NAVI_PREV) {
            movePrev(); // Move focus to the previous widget
            return true;
        } else if (event == FOCUS_MANAGER_NAVI_SELECT) {
            selectCurrent(); // Select the currently focused widget
            return true; // Always consume SELECT event to prevent it from propagating
        }
    }
    return false; // Event was not handled  
}

/**
 * @brief Check if the active widget has timed out.
 *
 * If the widget's timeout has expired, clear the active widget and mark the UI dirty.
 */
void FocusManager::checkActiveWidgetTimeout() {
    if (!m_activeWidget) return;

    uint32_t timeout = m_activeWidget->getTimeout();
    if (timeout == 0) return;

    uint32_t currentTime = m_ui.getCurrentTime();
    uint32_t lastInteractionTime = m_activeWidget->getLastInteractionTime();

    if (currentTime - lastInteractionTime >= timeout) {
        clearActiveWidget();
        m_ui.markDirty();
    }
}

/**
 * @brief Move focus to the next widget in the list.
 *
 * Animates the focus box to the new widget's position and size.
 */
void FocusManager::moveNext() {
    if (m_Widgets.empty()) {
        enterIdle(true);
        return;
    }

    m_ui.clearUnprotectedAnimations();
    const int old_index = index;
    const int next_index = (index == -1) ? 0 : (index + 1) % m_Widgets.size();

    if (next_index != old_index) {
        beginFocusAnimation(next_index);
    }
}

/**
 * @brief Move focus to the previous widget in the list.
 *
 * Animates the focus box to the new widget's position and size.
 */
void FocusManager::movePrev() {
    if (m_Widgets.empty()) {
        enterIdle(true);
        return;
    }

    m_ui.clearUnprotectedAnimations();
    const int old_index = index;
    const int next_index = (index == -1)
        ? static_cast<int>(m_Widgets.size()) - 1
        : (index - 1 + m_Widgets.size()) % m_Widgets.size();

    if (next_index != old_index) {
        beginFocusAnimation(next_index);
    }
}

/**
 * @brief Select the currently focused widget.
 *
 * If the widget returns true on selection, it becomes active.
 */
void FocusManager::selectCurrent() {
    last_focus_change_time = m_ui.getCurrentTime();

    if (index < 0 || index >= static_cast<int>(m_Widgets.size())) {
        return;
    }

    IWidget* selectedWidget = m_Widgets[index];
    if (selectedWidget->onSelect()) {
        m_activeWidget = selectedWidget;
        m_activeWidget->onActivate(m_ui.getCurrentTime());
        enterIdle(false);
    }

    const FocusBox target = selectedWidget->getFocusBox();
    m_ui.animate(m_current_focus_box.x, target.x, 100, EasingType::EASE_OUT_QUAD);
    m_ui.animate(m_current_focus_box.y, target.y, 100, EasingType::EASE_OUT_QUAD);
    m_ui.animate(m_current_focus_box.w, target.w, 100, EasingType::EASE_OUT_QUAD);
    m_ui.animate(m_current_focus_box.h, target.h, 100, EasingType::EASE_OUT_QUAD);
}

/**
 * @brief Draw the focus manager and handle animations.
 *
 * Handles active widget timeout, focus box expansion/contraction animations,
 * and draws the focus box on the U8G2 display.
 */
void FocusManager::draw() {
    // Update active widget timeout
    checkActiveWidgetTimeout();

    // Handle focus box shrink animation after inactivity
    if (m_state != State::IDLE && m_ui.getCurrentTime() - last_focus_change_time > 2500) {
        if (m_state != State::ANIMATING_SHRINK) {
            beginShrinkAnimation();
        }
    }

    if (m_state != State::IDLE) {
        if (m_state == State::ANIMATING_SHRINK && m_current_focus_box.w <= 1 && m_current_focus_box.h <= 1) {
            // Shrink complete
            enterIdle(true);
            return;
        }

        U8G2& u8g2 = m_ui.getU8G2();
        u8g2.setDrawColor(2); // Draw focus box with color 2

        if (index >= 0 && index < (int)m_Widgets.size()) {
            m_target_focus_box = m_Widgets[index]->getFocusBox();
        }

        if (m_state == State::ANIMATING && m_current_focus_box == m_target_focus_box) {
            enterFocused(false);
        }

        switch (m_state) {
            case State::ANIMATING:
            case State::FOCUSED:
            case State::ANIMATING_SHRINK:
                u8g2.drawBox(m_current_focus_box.x, m_current_focus_box.y,
                             m_current_focus_box.w, m_current_focus_box.h);
                break;
            default:
                break;
        }

        u8g2.setDrawColor(1); // Restore draw color
    }
}

/**
 * @brief Add a widget to the focus manager.
 * @param w Pointer to the widget to add
 */
void FocusManager::addWidget(IWidget* w) {
    m_Widgets.push_back(w);
}

/**
 * @brief Remove a widget from the focus manager.
 * @param w Pointer to the widget to remove
 */
void FocusManager::removeWidget(IWidget* w) {
    auto it = etl::find(m_Widgets.begin(), m_Widgets.end(), w);
    if (it != m_Widgets.end()) {
        m_Widgets.erase(it);
    }

    if (m_Widgets.empty()) {
        enterIdle(true);
    } else if (index >= (int)m_Widgets.size()) {
        index = m_Widgets.size() - 1;
        enterFocused(true);
    }
}
