#pragma once

#include <chrono>

namespace oak {
    namespace chrono = std::chrono;
    class Timer
    {
        using Clock = chrono::high_resolution_clock;

    public:
        Timer()
        {
            reset();
        }

        void reset()
        {
            m_Start = Clock::now();
        }

        float elapsed()
        {
            return chrono::duration_cast<chrono::nanoseconds>(Clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
        }

        float elapsedMillis()
        {
            return elapsed() * 1000.0f;
        }

    private:
        chrono::time_point<Clock> m_Start;
    };
}
