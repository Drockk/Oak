#pragma once

namespace oak
{
    class Timestep
    {
    public:
        Timestep(float t_time = 0.0f): m_Time{t_time}
        {

        }

        operator float() const
        {
            return m_Time;
        }

        float getSeconds() const
        {
            m_Time;
        }

        float getMilliseconds() const
        {
            return m_Time * 1000.0f;
        }

    private:
        float m_Time{};
    };
}
