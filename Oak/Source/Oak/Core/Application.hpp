#pragma once
#include <memory>

#include "Oak/Core/Window.hpp"
#include "Oak/Events/Event.hpp"

namespace oak
{
    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        void init();
        void run();
        void shutdown();

    private:
        void onEvent(const Event& t_event);

        std::unique_ptr<oak::Window> m_Window;
    };
}

std::unique_ptr<oak::Application> createApplication();
