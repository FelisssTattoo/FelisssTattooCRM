#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>

using namespace std::chrono_literals;

class Timer {
public:
    Timer();
    ~Timer();

    void setSingleShot(bool is_singleshot);
    void setCallback(std::function<void()> callback);
    void setInterval(int msec);
    void setInterval(std::chrono::milliseconds msec);
    void startOrReset();
    void stop();

private:
    void workerFunction();

    std::atomic<bool> doRunWorker;
    bool mIsSingleshot;

    std::function<void()> mCallback;
    std::mutex mStartPointMutex;
    std::chrono::time_point<std::chrono::steady_clock> mStartPoint;
    std::chrono::duration<int, std::milli> mDuration;
    std::thread mThread;

    std::chrono::duration<int, std::milli> TICK = 250ms;
};
