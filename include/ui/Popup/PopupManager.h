/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupInterface.h"
#include "etl/vector.h"
#include "etl/memory.h"
#include "config.h"

class PixelUI;

/**
 * @class PopupManager
 * @brief Manages the lifecycle, drawing, and input of multiple popups.
 */
class PopupManager {
private:
    etl::vector<etl::unique_ptr<IPopup>, MAX_POPUP_NUM> _popups;
    PixelUI& m_ui;
    
public:
    PopupManager(PixelUI& ui) : m_ui(ui) {}
    ~PopupManager() = default;

    void addPopup(etl::unique_ptr<IPopup> popup);
    void removePopup(IPopup* popup);
    void clearPopups();
    void drawPopups();
    void updatePopups(uint32_t currentTime);
    bool handleTopPopupInput(InputEvent event);
    size_t getPopupCounts() const { return _popups.size(); }
};
