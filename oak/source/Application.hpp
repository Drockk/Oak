#pragma once
#include <glad/gl.h>
#include "Event.hpp"
#include "Shader.hpp"
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

        GLuint m_VAO;
        GLuint m_VBO;

        std::shared_ptr<EventQueue> m_EventQueue;
        Shader m_Shader;
        Window m_Window;
    };
}
