/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "PopupInfo.h"
#include "PopupProgress.h"
#include "PopupValue4Digits.h"
#include "core/Callbacks.h"
#include "config.h"
#include <etl/queue.h>
#include <etl/utility.h>
#include <etl/variant_pool.h>

class PixelUI;

/**
 * @brief Owns one active Popup and a fixed-capacity FIFO of pending requests.
 */
class PopupManager {
private:
    static_assert(MAX_POPUP_NUM > 0, "Popup capacity must include one active slot");
    static constexpr size_t MAX_PENDING_POPUP_NUM = MAX_POPUP_NUM - 1U;

    enum class RequestType : uint8_t {
        Info,
        Progress,
        Value4Digits
    };

    struct Request {
        RequestType type = RequestType::Info;
        uint16_t width = 0;
        uint16_t height = 0;
        uint16_t duration = 0;
        const char* text = nullptr;
        const char* title = nullptr;
        int32_t* value = nullptr;
        int32_t minValue = 0;
        int32_t maxValue = 0;
        const uint8_t* font = nullptr;
        ValueCallback callback;
        bool useApparentValue = false;
    };

    using ActivePool = etl::variant_pool<
        1,
        PopupInfo,
        PopupProgress,
        PopupValue4Digits>;

    PixelUI& ui_;
    ActivePool activePool_;
    etl::queue<Request, MAX_PENDING_POPUP_NUM> requests_;
    IPopup* active_ = nullptr;
    bool dispatching_ = false;

    bool enqueue(Request&& request);
    void activateNext();
    void destroyActive();

public:
    explicit PopupManager(PixelUI& ui) : ui_(ui) {}
    ~PopupManager();

    PopupManager(const PopupManager&) = delete;
    PopupManager& operator=(const PopupManager&) = delete;

    bool enqueueInfo(uint16_t width, uint16_t height,
                     const char* text, const char* title, uint16_t duration,
                     const uint8_t* font = u8g2_font_wqy12_t_gb2312);
    bool enqueueProgress(uint16_t width, uint16_t height,
                         int32_t& value, int32_t minValue, int32_t maxValue,
                         const char* title, uint16_t duration,
                         ValueCallback callback = nullptr,
                         bool useApparentValue = false);
    bool enqueueValue4Digits(uint16_t width, uint16_t height,
                             int32_t& value, const char* title,
                             uint16_t duration,
                             ValueCallback callback = nullptr);

    void clearPopups();
    void drawPopups();
    void updatePopups(uint32_t currentTime);
    bool handleTopPopupInput(InputEvent event);

    size_t getPopupCounts() const {
        return requests_.size() + ((active_ != nullptr) ? 1U : 0U);
    }
    size_t pendingCount() const { return requests_.size(); }
    bool hasActivePopup() const { return active_ != nullptr; }
};
