/*
 * Copyright (C) 2025 Lawrence Link
 */

#pragma once

#include "core/Callbacks.h"
#include "core/NumericFormatter.h"
#include "core/NumericRange.h"
#include "core/ValueEdit.h"
#include "config.h"
#include <etl/queue.h>
#include <etl/utility.h>
#include <etl/variant.h>
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

struct RequestEnvelope {
    uint16_t width = 0;
    uint16_t height = 0;
    uint16_t duration = 0;
};

#if PIXELUI_USE_POPUP_INFO
struct InfoRequest {
    RequestEnvelope envelope{};
    // All pointers are non-owning and must survive pending plus active use.
    const char* text = nullptr;
    const char* title = nullptr;
    const uint8_t* font = nullptr;
};
#endif

#if PIXELUI_USE_POPUP_PROGRESS
struct ProgressRequest {
    RequestEnvelope envelope{};
    ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm;
    NumericRange range{};
    // Formatter and binding contexts are non-owning.
    NumericFormatter formatter{};
    ValueEditorBinding binding{};
    const char* title = nullptr;
    // The callback object is owned, but any references captured by it are not.
    ValueCallback callback{};
};
#endif

#if PIXELUI_USE_POPUP_VALUE_DIGITS
struct ValueDigitsRequest {
    RequestEnvelope envelope{};
    uint8_t digitCount = 0U;
    ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm;
    // Binding contexts and title are non-owning.
    ValueEditorBinding binding{};
    const char* title = nullptr;
    // The callback object is owned, but any references captured by it are not.
    ValueCallback callback{};
};
#endif

using PopupRequest = etl::variant<
#if PIXELUI_USE_POPUP_INFO
    InfoRequest
    #if PIXELUI_USE_POPUP_PROGRESS || PIXELUI_USE_POPUP_VALUE_DIGITS
    ,
    #endif
#endif
#if PIXELUI_USE_POPUP_PROGRESS
    ProgressRequest
    #if PIXELUI_USE_POPUP_VALUE_DIGITS
    ,
    #endif
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
    ValueDigitsRequest
#endif
>;

/**
 * @brief Owns one active Popup and a fixed-capacity FIFO of pending requests.
 *
 * Non-owning request targets must remain valid until the request is removed or
 * its active Popup is destroyed. clearPopups() ends that required lifetime.
 */
class PopupManager {
private:
    static_assert(MAX_POPUP_NUM > 0, "Popup capacity must include one active slot");
    static constexpr size_t MAX_PENDING_POPUP_NUM = MAX_POPUP_NUM - 1U;

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
    etl::queue<PopupRequest, MAX_PENDING_POPUP_NUM> requests_;
    IPopup* active_ = nullptr;
    bool dispatching_ = false;

    static bool validEnvelope(const RequestEnvelope& envelope);
    bool enqueue(PopupRequest&& request);
    void activateNext();
    void destroyActive();
#if PIXELUI_USE_POPUP_INFO
    void activate(InfoRequest& request);
#endif
#if PIXELUI_USE_POPUP_PROGRESS
    void activate(ProgressRequest& request);
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
    void activate(ValueDigitsRequest& request);
#endif

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
                         ValueEditorBinding binding,
                         const NumericRange& range,
                         NumericFormatter formatter,
                         const char* title, uint16_t duration,
                         ValueCallback callback = nullptr,
                         ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm);
    bool enqueueProgress(uint16_t width, uint16_t height,
                         int32_t& value, int32_t minValue, int32_t maxValue,
                         const char* title, uint16_t duration,
                         ValueCallback callback = nullptr);
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
    bool enqueueValueDigits(uint16_t width, uint16_t height,
                            ValueEditorBinding binding, uint8_t digitCount,
                            const char* title, uint16_t duration,
                            ValueCallback callback = nullptr,
                            ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm);
    bool enqueueValueDigits(uint16_t width, uint16_t height,
                            int32_t& value, uint8_t digitCount,
                            const char* title, uint16_t duration,
                            ValueCallback callback = nullptr);
#endif

    void clearPopups();
    void drawPopups();
    void updatePopups(uint32_t currentTime);
    uint32_t nextWakeupMs(uint32_t currentTime, uint32_t frameIntervalMs) const;
    bool handleTopPopupInput(InputEvent event);

    size_t getPopupCounts() const {
        return requests_.size() + ((active_ != nullptr) ? 1U : 0U);
    }
    size_t pendingCount() const { return requests_.size(); }
    bool hasActivePopup() const { return active_ != nullptr; }
};

#endif
