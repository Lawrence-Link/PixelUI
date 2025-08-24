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