#include "oakpch.hpp"
#include "Oak/Core/Application.hpp"

#include "Oak/Core/Log.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Oak/Scripting/ScriptEngine.hpp"

#include "Oak/Core/Input.hpp"
#include "Oak/Utils/PlatformUtils.hpp"

namespace oak {
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& specification): m_Specification(specification)
    {
        OAK_PROFILE_FUNCTION();

        OAK_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // Set working directory here
        if (!m_Specification.workingDirectory.empty()) {
            std::filesystem::current_path(m_Specification.workingDirectory);
        }

        m_Window = Window::create(WindowProps(m_Specification.name));
        m_Window->setEventCallback(OAK_BIND_EVENT_FN(Application::onEvent));

        Renderer::init();

        m_ImGuiLayer = new ImGuiLayer();
        pushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        OAK_PROFILE_FUNCTION();

        ScriptEngine::shutdown();
        Renderer::shutdown();
    }

    void Application::pushLayer(Layer* layer)
    {
        OAK_PROFILE_FUNCTION();

        m_LayerStack.pushLayer(layer);
        layer->onAttach();
    }

    void Application::pushOverlay(Layer* layer)
    {
        OAK_PROFILE_FUNCTION();

        m_LayerStack.pushOverlay(layer);
        layer->onAttach();
    }

    void Application::close()
    {
        m_Running = false;
    }

    void Application::submitToMainThread(const std::function<void()>& function)
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        m_MainThreadQueue.emplace_back(function);
    }

    void Application::onEvent(oak::Event& e)
    {
        OAK_PROFILE_FUNCTION();

        oak::EventDispatcher dispatcher(e);
        dispatcher.dispatch<oak::WindowCloseEvent>(OAK_BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<oak::WindowResizeEvent>(OAK_BIND_EVENT_FN(Application::onWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (e.Handled) {
                break;
            }
            (*it)->onEvent(e);
        }
    }

    void Application::run()
    {
        OAK_PROFILE_FUNCTION();

        while (m_Running)
        {
            OAK_PROFILE_SCOPE("RunLoop");

            auto time = oak::Time::getTime();
            oak::Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            executeMainThreadQueue();

            if (!m_Minimized) {
                {
                    OAK_PROFILE_SCOPE("LayerStack OnUpdate");

                    for (auto* layer : m_LayerStack) {
                        layer->onUpdate(timestep);
                    }
                }

                m_ImGuiLayer->begin();
                {
                    OAK_PROFILE_SCOPE("LayerStack OnImGuiRender");

                    for (auto* layer : m_LayerStack) {
                        layer->onImGuiRender();
                    }
                }
                m_ImGuiLayer->end();
            }

            m_Window->onUpdate();
        }
    }

    bool Application::onWindowClose(oak::WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::onWindowResize(oak::WindowResizeEvent& e)
    {
        OAK_PROFILE_FUNCTION();

        if (e.getWidth() == 0 || e.getHeight() == 0) {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::onWindowResize(e.getWidth(), e.getHeight());

        return false;
    }

    void Application::executeMainThreadQueue()
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        for (auto& func : m_MainThreadQueue) {
            func();
        }

        m_MainThreadQueue.clear();
    }
}
