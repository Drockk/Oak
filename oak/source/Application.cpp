#include "Application.hpp"

namespace oak
{
    Application::Application(const std::string& name)
    {
        m_Window.createWindow(name, 800, 600);
    }

    void Application::init()
    {
        //In the faeture.
    }

    void Application::run()
    {
        while (m_Running)
        {
            m_Window.onUpdate();
        }
    }

    void Application::shutdown()
    {
        m_Window.onShutdown();
    }
}
