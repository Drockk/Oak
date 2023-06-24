#include "oakpch.hpp"

#include "Oak/Core/Application.hpp"

namespace oak
{
    void Application::init()
    {
        oak::Log::init();

        m_Window = Window::create({ "Oak", {1280, 720}, [this](const Event& t_event) { onEvent(t_event); } });
    }

    void Application::run()
    {
        try
        {
            m_Window->run();
        }
        catch (const std::exception& e)
        {
            OAK_CORE_CRITICAL(e.what());
        }
    }

    void Application::shutdown()
    {
        OAK_CORE_INFO("Shutingdown Oak");
    }

    void Application::onEvent(const Event& t_event)
    {
        OAK_CORE_INFO("EVENT!");
    }
}
