#pragma once
#include "Oak/Events/Event.hpp"

#include <string>

namespace oak
{
    struct WindowData
    {
        std::string name;
        std::pair<uint32_t, uint32_t> resolution;
        std::function<void(const Event&)> onEvent;
    };

    class Window
    {
    public:
        Window() = delete;
        Window(WindowData t_data): m_Data(t_data) {}
        virtual ~Window() = default;

        virtual void run() = 0;

        static std::unique_ptr<Window> create(WindowData t_data);

    protected:
        WindowData m_Data;
    };
}
