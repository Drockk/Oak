#include "oakpch.hpp"

#include "Oak/Core/Application.hpp"

namespace oak
{
    void Application::init()
    {
        oak::Log::init();
        OAK_CORE_INFO("I am alive!");

        m_Window = Window::create("Oak", {1280, 720});
    }

    void Application::run()
    {
        m_Window->run();
    }

    void Application::shutdown()
    {

    }
}
