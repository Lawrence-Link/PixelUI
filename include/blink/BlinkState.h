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

#pragma once

#include "core/scheduling/UiDeadlineScheduler.h"
#include <stdint.h>

class PixelUI;

class BlinkState final : private UiDeadlineSource {
public:
    explicit BlinkState(PixelUI& ui, uint32_t intervalMs = 500U);
    ~BlinkState() override = default;

    void start();
    void stop();
    void stopWhenVisible();
    void setInterval(uint32_t intervalMs);

    bool isVisible() const { return visible_; }
    bool isRunning() const { return running_; }
    uint32_t interval() const { return intervalMs_; }

private:
    uint32_t nextWakeupMs(uint32_t currentTime) const override;
    bool update(uint32_t currentTime) override;

    PixelUI& ui_;
    uint32_t intervalMs_;
    uint32_t lastToggleMs_;
    bool visible_ = true;
    bool running_ = false;
    bool stopWhenVisible_ = false;
};
