#pragma once

#include "Oak/Core/Base.hpp"

#include "Oak/Core/Window.hpp"
#include "Oak/Core/LayerStack.hpp"
#include "Oak/Events/Event.hpp"
#include "Oak/Events/ApplicationEvent.hpp"

#include "Oak/Core/Timestep.hpp"

#include "Oak/ImGui/ImGuiLayer.hpp"

namespace oak
{
    struct ApplicationCommandLineArgs
    {
        int count = 0;
        char** args = nullptr;

        const char* operator[](int t_index) const
        {
            OAK_CORE_ASSERT(t_index < count);
            return args[t_index];
        }
    };

    struct ApplicationSpecification
    {
        std::string name = "Oak Application";
        std::string workingDirectory;
        ApplicationCommandLineArgs commandLineArgs;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& t_specification);
        virtual ~Application() = default;

        void onEvent(Event& t_event);

        void pushLayer(Layer* t_layer);
        void pushOverlay(Layer* t_layer);

        Window& getWindow() { return *m_Window; }

        void run();
        void shutdown();

        void close();

        ImGuiLayer* getImGuiLayer() { return m_ImGuiLayer; }

        static Application& get() { return *s_Instance; }

        const ApplicationSpecification& getSpecification() const { return m_Specification; }

        void submitToMainThread(const std::function<void()>& t_function);
    private:
        bool onWindowClose(WindowCloseEvent& t_event);
        bool onWindowResize(WindowResizeEvent& t_event);

        void executeMainThreadQueue();
    private:
        ApplicationSpecification m_Specification;
        Scope<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running{true};
        bool m_Minimized{false};
        LayerStack m_LayerStack;
        float m_LastFrameTime{0.0f};

        std::vector<std::function<void()>> m_MainThreadQueue;
        std::mutex m_MainThreadQueueMutex;
    private:
        inline static Application* s_Instance{nullptr};
    };

    // To be defined in CLIENT
    std::unique_ptr<Application> createApplication(ApplicationCommandLineArgs t_args);
}
