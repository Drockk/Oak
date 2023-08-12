#pragma once

#include "Oak/Core/Base.hpp"

#include "Oak/Core/Window.hpp"
#include "Oak/Core/LayerStack.hpp"
#include "Oak/Events/Event.hpp"
#include "Oak/Events/ApplicationEvent.hpp"

#include "Oak/Core/Timestep.hpp"

#include "Oak/ImGui/ImGuiLayer.hpp"

int main(int argc, char** argv);

namespace oak {
    struct ApplicationCommandLineArgs
    {
        int count = 0;
        char** args = nullptr;

        const char* operator[](int index) const
        {
            OAK_CORE_ASSERT(index < count);
            return args[index];
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
        Application(const ApplicationSpecification& specification);
        virtual ~Application();

        void onEvent(oak::Event& e);

        void pushLayer(oak::Layer* layer);
        void pushOverlay(oak::Layer* layer);

        oak::Window& getWindow() { return *m_Window; }

        void close();

        oak::ImGuiLayer* getImGuiLayer() { return m_ImGuiLayer; }

        static Application& get() { return *s_Instance; }

        const ApplicationSpecification& getSpecification() const { return m_Specification; }

        void submitToMainThread(const std::function<void()>& function);

    private:
        void run();
        bool onWindowClose(oak::WindowCloseEvent& e);
        bool onWindowResize(oak::WindowResizeEvent& e);

        void executeMainThreadQueue();

        ApplicationSpecification m_Specification;
        oak::Scope<oak::Window> m_Window;
        oak::ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        bool m_Minimized = false;
        oak::LayerStack m_LayerStack;
        float m_LastFrameTime = 0.0f;

        std::vector<std::function<void()>> m_MainThreadQueue;
        std::mutex m_MainThreadQueueMutex;

        static Application* s_Instance;
        friend int ::main(int argc, char** argv);
    };
}

// To be defined in CLIENT
oak::Application* createApplication(oak::ApplicationCommandLineArgs args);
