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

#include <stdint.h>

class UiDeadlineScheduler;

class UiDeadlineSource {
public:
    virtual ~UiDeadlineSource();

    UiDeadlineSource(const UiDeadlineSource&) = delete;
    UiDeadlineSource& operator=(const UiDeadlineSource&) = delete;
    UiDeadlineSource(UiDeadlineSource&&) = delete;
    UiDeadlineSource& operator=(UiDeadlineSource&&) = delete;

protected:
    explicit UiDeadlineSource(UiDeadlineScheduler& scheduler);

private:
    friend class UiDeadlineScheduler;

    virtual uint32_t nextWakeupMs(uint32_t currentTime) const = 0;
    virtual bool update(uint32_t currentTime) = 0;

    UiDeadlineScheduler* scheduler_ = nullptr;
    UiDeadlineSource* next_ = nullptr;
};

class UiDeadlineScheduler {
public:
    UiDeadlineScheduler() = default;
    ~UiDeadlineScheduler();

    UiDeadlineScheduler(const UiDeadlineScheduler&) = delete;
    UiDeadlineScheduler& operator=(const UiDeadlineScheduler&) = delete;

    bool update(uint32_t currentTime);
    uint32_t nextWakeupMs(uint32_t currentTime) const;

private:
    friend class UiDeadlineSource;

    void attach(UiDeadlineSource& source);
    void detach(UiDeadlineSource& source);

    UiDeadlineSource* first_ = nullptr;
};
