/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "core/Callbacks.h"
#include "config.h"
#include <etl/queue.h>
#include <etl/utility.h>
#include <etl/variant_pool.h>

#if PIXELUI_USE_POPUP_INFO
#include "PopupInfo.h"
#endif

#if PIXELUI_USE_POPUP_PROGRESS
#include "PopupProgress.h"
#endif

#if PIXELUI_USE_POPUP_VALUE_DIGITS
#include "PopupValueDigits.h"
#endif

class PixelUI;

#if PIXELUI_USE_POPUP

/**
 * @brief Owns one active Popup and a fixed-capacity FIFO of pending requests.
 */
class PopupManager {
private:
    static_assert(MAX_POPUP_NUM > 0, "Popup capacity must include one active slot");
    static constexpr size_t MAX_PENDING_POPUP_NUM = MAX_POPUP_NUM - 1U;

    enum class RequestType : uint8_t {
        None,
#if PIXELUI_USE_POPUP_INFO
        Info,
#endif
#if PIXELUI_USE_POPUP_PROGRESS
        Progress,
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
        ValueDigits
#endif
    };

    struct Request {
        RequestType type = RequestType::None;
        uint16_t width = 0;
        uint16_t height = 0;
        uint16_t duration = 0;
#if PIXELUI_USE_POPUP_INFO
        const char* text = nullptr;
#endif
        const char* title = nullptr;
#if PIXELUI_USE_POPUP_PROGRESS || PIXELUI_USE_POPUP_VALUE_DIGITS
        int32_t* value = nullptr;
#endif
#if PIXELUI_USE_POPUP_PROGRESS
        int32_t minValue = 0;
        int32_t maxValue = 0;
#endif
#if PIXELUI_USE_POPUP_INFO
        const uint8_t* font = nullptr;
#endif
#if PIXELUI_USE_POPUP_PROGRESS || PIXELUI_USE_POPUP_VALUE_DIGITS
        ValueCallback callback;
#endif
#if PIXELUI_USE_POPUP_PROGRESS
        bool useApparentValue = false;
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
        uint8_t digitCount = 0;
#endif
    };

    using ActivePool = etl::variant_pool<
        1
#if PIXELUI_USE_POPUP_INFO
        , PopupInfo
#endif
#if PIXELUI_USE_POPUP_PROGRESS
        , PopupProgress
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
        , PopupValueDigits
#endif
    >;

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

#if PIXELUI_USE_POPUP_INFO
    bool enqueueInfo(uint16_t width, uint16_t height,
                     const char* text, const char* title, uint16_t duration,
                     const uint8_t* font = PIXELUI_FONT_TEXT);
#endif
#if PIXELUI_USE_POPUP_PROGRESS
    bool enqueueProgress(uint16_t width, uint16_t height,
                         int32_t& value, int32_t minValue, int32_t maxValue,
                         const char* title, uint16_t duration,
                         ValueCallback callback = nullptr,
                         bool useApparentValue = false);
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
    bool enqueueValueDigits(uint16_t width, uint16_t height,
                            int32_t& value, uint8_t digitCount,
                            const char* title, uint16_t duration,
                            ValueCallback callback = nullptr);
#endif

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

#endif
