/*
 * Copyright (C) 2025 Lawrence Link
 */

#include "ui/Popup/PopupManager.h"
#if PIXELUI_USE_POPUP
#include "PixelUI.h"

PopupManager::~PopupManager() {
    clearPopups();
}

bool PopupManager::enqueue(Request&& request) {
    if (dispatching_ || (getPopupCounts() >= MAX_POPUP_NUM)) {
        return false;
    }

    requests_.push(etl::move(request));
    if (active_ == nullptr) {
        activateNext();
    }
    return active_ != nullptr;
}

#if PIXELUI_USE_POPUP_INFO
bool PopupManager::enqueueInfo(uint16_t width, uint16_t height,
                               const char* text, const char* title,
                               uint16_t duration, const uint8_t* font) {
    if (text == nullptr) {
        return false;
    }

    Request request;
    request.type = RequestType::Info;
    request.width = width;
    request.height = height;
    request.duration = duration;
    request.text = text;
    request.title = title;
    request.font = font;
    return enqueue(etl::move(request));
}
#endif

#if PIXELUI_USE_POPUP_PROGRESS
bool PopupManager::enqueueProgress(uint16_t width, uint16_t height,
                                   int32_t& value, int32_t minValue,
                                   int32_t maxValue, const char* title,
                                   uint16_t duration, ValueCallback callback,
                                   bool useApparentValue) {
    Request request;
    request.type = RequestType::Progress;
    request.width = width;
    request.height = height;
    request.duration = duration;
    request.title = title;
    request.value = &value;
    request.minValue = minValue;
    request.maxValue = maxValue;
    request.callback = etl::move(callback);
    request.useApparentValue = useApparentValue;
    return enqueue(etl::move(request));
}
#endif

#if PIXELUI_USE_POPUP_VALUE_DIGITS
bool PopupManager::enqueueValueDigits(uint16_t width, uint16_t height,
                                      int32_t& value, uint8_t digitCount,
                                      const char* title, uint16_t duration,
                                      ValueCallback callback) {
    if (!PopupValueDigits::isValidDigitCount(digitCount)) return false;
    Request request;
    request.type = RequestType::ValueDigits;
    request.width = width;
    request.height = height;
    request.duration = duration;
    request.title = title;
    request.value = &value;
    request.digitCount = digitCount;
    request.callback = etl::move(callback);
    return enqueue(etl::move(request));
}
#endif

void PopupManager::activateNext() {
    if ((active_ != nullptr) || requests_.empty()) {
        return;
    }

    Request request = etl::move(requests_.front());
    requests_.pop();

    dispatching_ = true;
    switch (request.type) {
#if PIXELUI_USE_POPUP_INFO
        case RequestType::Info:
            active_ = activePool_.create<PopupInfo>(
                ui_, request.width, request.height, request.text,
                request.title, request.duration, request.font);
            break;
#endif
#if PIXELUI_USE_POPUP_PROGRESS
        case RequestType::Progress:
            active_ = activePool_.create<PopupProgress>(
                ui_, request.width, request.height, *request.value,
                request.minValue, request.maxValue, request.title,
                request.duration, etl::move(request.callback),
                request.useApparentValue);
            break;
#endif
#if PIXELUI_USE_POPUP_VALUE_DIGITS
        case RequestType::ValueDigits:
            active_ = activePool_.create<PopupValueDigits>(
                ui_, request.width, request.height, *request.value, request.digitCount,
                request.title, request.duration, etl::move(request.callback));
            break;
#endif
        case RequestType::None:
            break;
    }
    dispatching_ = false;
}

void PopupManager::destroyActive() {
    if (active_ == nullptr) {
        return;
    }

    IPopup* popup = active_;
    active_ = nullptr;
    dispatching_ = true;
    activePool_.destroy<IPopup>(popup);
    dispatching_ = false;
}

void PopupManager::clearPopups() {
    if (dispatching_) {
        return;
    }

    requests_.clear();
    destroyActive();
}

void PopupManager::drawPopups() {
    if (active_ == nullptr) {
        return;
    }

    dispatching_ = true;
    active_->draw();
    dispatching_ = false;
}

void PopupManager::updatePopups(uint32_t currentTime) {
    if (active_ == nullptr) {
        activateNext();
        return;
    }

    dispatching_ = true;
    const bool active = active_->update(currentTime);
    dispatching_ = false;
    if (!active) {
        destroyActive();
        activateNext();
    }
}

bool PopupManager::handleTopPopupInput(InputEvent event) {
    if (active_ == nullptr) {
        return false;
    }

    dispatching_ = true;
    const bool handled = active_->handleInput(event);
    dispatching_ = false;
    return handled;
}
#endif
