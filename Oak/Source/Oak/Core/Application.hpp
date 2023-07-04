#pragma once
#include <memory>

#include "Oak/Core/Window.hpp"
#include "Oak/Events/Event.hpp"
#include "Oak/Events/ApplicationEvent.hpp"

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
        void onEvent(Event& t_event);
        bool onWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);

        bool m_Running{ true };
        bool m_Minimized{ false };

        std::unique_ptr<oak::Window> m_Window;
    };
}

std::unique_ptr<oak::Application> createApplication();
