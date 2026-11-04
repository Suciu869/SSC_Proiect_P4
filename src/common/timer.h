#pragma once

#include <chrono>

class Timer {
public:
    Timer() { start(); }

    void start() { begin_ = Clock::now(); }

    double elapsed_ns() const {
        return std::chrono::duration<double, std::nano>(Clock::now() - begin_).count();
    }

private:
    using Clock = std::chrono::steady_clock;
    static_assert(Clock::is_steady, "timer-ul trebuie sa fie monoton");

    Clock::time_point begin_;
};
