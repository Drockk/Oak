#include "oakpch.hpp"
#include "Oak/Core/Application.hpp"

#include "Oak/Core/Log.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Oak/Scripting/ScriptEngine.hpp"

#include "Oak/Core/Input.hpp"
#include "Oak/Utils/PlatformUtils.hpp"

namespace oak
{
    Application::Application(const ApplicationSpecification& t_specification)
        : m_Specification(t_specification)
    {
        OAK_PROFILE_FUNCTION();

        OAK_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // Set working directory here
        if (!m_Specification.workingDirectory.empty())
        {
            std::filesystem::current_path(m_Specification.workingDirectory);
        }

        m_Window = Window::create(WindowProps(m_Specification.name));
        m_Window->setEventCallback(OAK_BIND_EVENT_FN(Application::onEvent));

        Renderer::init();

        m_ImGuiLayer = new ImGuiLayer();
        pushOverlay(m_ImGuiLayer);
    }

    void Application::shutdown()
    {
        OAK_PROFILE_FUNCTION();
        ScriptEngine::Shutdown();
        Renderer::Shutdown();
    }

    void Application::pushLayer(Layer* t_layer)
    {
        OAK_PROFILE_FUNCTION();

        m_LayerStack.pushLayer(t_layer);
        t_layer->onAttach();
    }

    void Application::pushOverlay(Layer* t_layer)
    {
        OAK_PROFILE_FUNCTION();

        m_LayerStack.PushOverlay(t_layer);
        t_layer->onAttach();
    }

    void Application::close()
    {
        m_Running = false;
    }

    void Application::submitToMainThread(const std::function<void()>& t_function)
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        m_MainThreadQueue.emplace_back(t_function);
    }

    void Application::onEvent(Event& t_event)
    {
        OAK_PROFILE_FUNCTION();

        EventDispatcher dispatcher(t_event);
        dispatcher.dispatch<WindowCloseEvent>(OAK_BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(OAK_BIND_EVENT_FN(Application::onWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (t_event.handled)
                break;
            (*it)->onEvent(t_event);
        }
    }

    void Application::run()
    {
        OAK_PROFILE_FUNCTION();

        while (m_Running)
        {
            OAK_PROFILE_SCOPE("RunLoop");

            float time = Time::GetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            executeMainThreadQueue();

            if (!m_Minimized)
            {
                {
                    OAK_PROFILE_SCOPE("LayerStack onUpdate");

                    for (Layer* layer : m_LayerStack)
                        layer->onUpdate(timestep);
                }

                m_ImGuiLayer->Begin();
                {
                    OAK_PROFILE_SCOPE("LayerStack onImGuiRender");

                    for (Layer* layer : m_LayerStack)
                        layer->onImGuiRender();
                }
                m_ImGuiLayer->End();
            }

            m_Window->onUpdate();
        }
    }

    bool Application::onWindowClose(WindowCloseEvent& t_event)
    {
        m_Running = false;
        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent& t_event)
    {
        OAK_PROFILE_FUNCTION();

        if (t_event.getWidth() == 0 || t_event.getHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::onWindowResize(t_event.getWidth(), t_event.getHeight());

        return false;
    }

    void Application::executeMainThreadQueue()
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        for (auto& func : m_MainThreadQueue)
            func();

        m_MainThreadQueue.clear();
    }
}
