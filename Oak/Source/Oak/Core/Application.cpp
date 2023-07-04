#include "oakpch.hpp"

#include "Oak/Core/Application.hpp"

#define OAK_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace oak
{
    void Application::init()
    {
        oak::Log::init();

        m_Window = Window::create({ "Oak", {1280, 720}, [this](Event& t_event) { onEvent(t_event); } });
    }

    void Application::run()
    {
        while (m_Running) {
            if (!m_Minimized) {
                //TODO: In the future
            }

            m_Window->onUpdate();
        }
    }

    void Application::shutdown()
    {
        OAK_CORE_INFO("Shutingdown Oak");
    }

    void Application::onEvent(Event& t_event)
    {
        EventDispatcher eventDispatcher(t_event);
        eventDispatcher.dispatch<WindowCloseEvent>(OAK_BIND_EVENT_FN(Application::onWindowClose));
        eventDispatcher.dispatch<WindowResizeEvent>(OAK_BIND_EVENT_FN(Application::onWindowResize));
    }

    bool Application::onWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }
    bool Application::onWindowResize(WindowResizeEvent& e)
    {
        if (e.getWidth() == 0 || e.getHeight() == 0) {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        return false;
    }
}
