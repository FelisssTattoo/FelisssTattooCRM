#include "timer.h"

Timer::Timer() : doRunWorker(false), mIsSingleshot(false) {}

Timer::~Timer() {
    stop();
}

void Timer::setSingleShot(bool is_singleshot) {
    mIsSingleshot = is_singleshot;
}

void Timer::setCallback(std::function<void()> callback) {
    mCallback = callback;
}

void Timer::setInterval(int msec) {
    mDuration = std::chrono::milliseconds(msec);
}

void Timer::setInterval(std::chrono::milliseconds msec) {
    mDuration = msec;
}

void Timer::startOrReset() {
    mStartPoint = std::chrono::steady_clock::now();
    doRunWorker.store(true);
    if (!mThread.joinable()) {
        mThread = std::thread(&Timer::workerFunction, this);
    }
}

void Timer::stop() {
    doRunWorker.store(false);
    if (mThread.joinable()) {
        mThread.join();
    }
}

void Timer::workerFunction() {
    while (doRunWorker.load()) {
        const auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(mStartPointMutex);
        if ((now - mStartPoint) >= mDuration) {
            mCallback();
            if (mIsSingleshot) {
                doRunWorker.store(false);
            } else {
                mStartPoint = now;
            }
        }
        std::this_thread::sleep_for(TICK);
    }
}
