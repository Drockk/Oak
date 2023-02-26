#pragma once
#include <glad/gl.h>
#include "Event.hpp"
#include "Window.hpp"

namespace oak
{
    class Application
    {
    public:
        Application(const std::string& name);
        ~Application() = default;

        void init();
        void run();
        void shutdown();

    private:
        bool m_Running{ true };

        GLuint m_ShaderProgram{ 0 };
        GLuint m_VAO{ 0 };
        GLuint m_VBO{ 0 };

        std::shared_ptr<EventQueue> m_EventQueue;
        Window m_Window;
    };
}
