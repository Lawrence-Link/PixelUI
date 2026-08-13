/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupInterface.h"

class PixelUI;

struct PopupContentBounds {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t centerX;
    int16_t centerY;
};

/**
 * @class PopupBase
 * @brief Base class for all popups, providing common functionality.
 */
class PopupBase : public IPopup {
protected:
    PixelUI& ui() const { return m_ui; }
    bool isClosing() const { return m_state == PopupState::CLOSING; }
    void requestClose();
    void resetAutoCloseTimer();
    void setContentHeight(uint16_t height) { m_height = height; }
    uint16_t popupWidth() const { return m_width; }
    uint16_t popupHeight() const { return m_height; }
    void setContentClip(const PopupContentBounds& bounds);

    virtual void drawContent(const PopupContentBounds& bounds) = 0;
    virtual bool handleContentInput(InputEvent event) { (void)event; return false; }
    virtual void onShown() {}
    virtual void onClosing() {}

public:
    PopupBase(PixelUI& ui, uint16_t width, uint16_t height, uint8_t priority, uint16_t duration);
    virtual ~PopupBase() = default;
    
    uint8_t getPriority() const override { return m_priority; }
    uint16_t getDuration() const override { return m_duration; }
    bool update(uint32_t currentTime) final;
    void draw() final;
    bool handleInput(InputEvent event) final;

private:
    static constexpr uint32_t TRANSITION_DURATION = 300U;
    static constexpr int16_t BORDER_OFFSET = 2;
    static constexpr int16_t BORDER_WIDTH = 1;

    void drawPopupBox(const PopupContentBounds& bounds);
    void resetClipWindow();
    void beginAppearing(uint32_t currentTime);
    bool updateAppearing(uint32_t currentTime);
    bool updateClosing(uint32_t currentTime);

    PixelUI& m_ui;
    uint16_t m_width;
    uint16_t m_height;
    uint8_t m_priority;
    uint16_t m_duration;
    uint32_t m_stateStartTime = 0;
    uint32_t m_transitionStartTime = 0;
    int32_t m_transitionStartSize = 0;
    int32_t m_currentBoxSize = 0;
    int32_t m_targetBoxSize = 0;
    PopupState m_state = PopupState::APPEARING;
    bool m_started = false;
};
