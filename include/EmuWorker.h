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