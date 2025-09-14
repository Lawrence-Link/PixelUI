/*
 * Copyright (C) 2025 Lawrence Link
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

#pragma once
#include <QObject>
#include <QThread>
#include <atomic>
#include <chrono>
#include <thread>

class EmuWorker : public QObject {
    Q_OBJECT
public:
    explicit EmuWorker(QObject *parent = nullptr);
    ~EmuWorker();
    
    void start();
    void stop();

    virtual void grandLoop() {  // the pseudo main loop
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            emit updateRequested();
        }
    }

signals:
    void updateRequested();

protected:
    std::atomic<bool> running{false};

private:
    std::thread workerThread;
};