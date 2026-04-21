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

#include "blinker/Blinker.h"
#include "PixelUI.h"
#include <cassert>

Blinker::Blinker(PixelUI& ui, uint32_t interval_ms)
    : m_ui(ui),
      m_interval_ms(interval_ms),
      m_last_toggle_ms(m_ui.getCurrentTime()),
      m_visible(true),
      m_running(false),
      m_stop_on_visible(false)
{
    if (m_interval_ms == 0) m_interval_ms = 500;
}

void Blinker::start() {
    if (!m_running) {
        m_running = true;
        m_last_toggle_ms = m_ui.getCurrentTime();
        m_stop_on_visible = false;
    }
}

void Blinker::stop() {
    m_running = false;
    m_visible = false;
    m_stop_on_visible = false;
}

void Blinker::stopOnVisible() {
    if (m_visible) {
        m_running = false;
        m_stop_on_visible = false;
        return;
    }
    m_stop_on_visible = true;
    m_running = true;
    m_last_toggle_ms = m_ui.getCurrentTime();
}

void Blinker::set_interval(uint32_t interval_ms) {
    m_interval_ms = (interval_ms == 0) ? 1 : interval_ms;
}

void Blinker::update() {
    if (!m_running) return;

    uint32_t now = m_ui.getCurrentTime();
    uint32_t delta = now - m_last_toggle_ms;

    if (m_interval_ms == 0) {
        m_visible = true;
        if (m_stop_on_visible && m_visible) {
            m_running = false;
            m_stop_on_visible = false;
        }
        return;
    }

    if (delta >= m_interval_ms) {
        uint32_t toggles = delta / m_interval_ms;
        if (toggles & 1u) {
            m_visible = !m_visible;
        }
        m_last_toggle_ms += toggles * m_interval_ms;

        if (m_stop_on_visible && m_visible) {
            m_running = false;
            m_stop_on_visible = false;
        }
    }
}

bool Blinker::is_visible() const {
    return m_visible;
}

bool Blinker::is_running() const {
    return m_running;
}
