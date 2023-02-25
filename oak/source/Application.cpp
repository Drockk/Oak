#include "Application.hpp"

#include <iostream>

namespace oak
{
    Application::Application(const std::string& name)
    {
        m_EventQueue = std::make_shared<EventQueue>();
        m_Window.createWindow(name, 800, 600, m_EventQueue);
    }

    void Application::init()
    {
        m_EventQueue->registerEventListener(EventType::WindowClose, [this](const EventPointer&) {
            m_Running = false;
        });

        //Glad: Load all OpenGL function pointers
        auto version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
        {
            std::cerr << "Failed to initialize OpenGL context" << std::endl;
        }

        std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

        glViewport(0, 0, 800, 600);
    }

    void Application::run()
    {
        while (m_Running)
        {
            //Render here.
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            m_Window.onUpdate();
            m_EventQueue->proccess();
        }
    }

    void Application::shutdown()
    {
        m_Window.onShutdown();
    }
}
