#pragma once
#include <glad/gl.h>
#include "Event.hpp"
#include "Window.hpp"

namespace oak
{
    class Application
    {
    public:
        Application(const std::string& name);
        ~Application() = default;

        void init();
        void run();
        void shutdown();

    private:
        bool m_Running{ true };

        std::shared_ptr<EventQueue> m_EventQueue;
        Window m_Window;
    };
}
