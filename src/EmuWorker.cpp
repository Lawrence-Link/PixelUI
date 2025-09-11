/*
 * Copyright (C) 2025 Lawrence Li
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "EmuWorker.h"

EmuWorker::EmuWorker(QObject *parent) : QObject(parent), running(false) {}

EmuWorker::~EmuWorker() {
    stop();
}

void EmuWorker::start() {
    if (running) return;  // Already running
    running = true;
    workerThread = std::thread(&EmuWorker::grandLoop, this);
}

void EmuWorker::stop() {
    if (!running) return;  // Not running
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();  // Wait for the thread to finish
    }
}