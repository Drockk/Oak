#pragma once
#include <glad/gl.h>

#include "Camera.hpp"
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

        GLuint m_CubeVAO;
        GLuint m_LightCubeVAO;
        GLuint m_VBO;

        Camera m_Camera;
        std::shared_ptr<EventQueue> m_EventQueue;
        Shader m_LightingShader;
        Shader m_LightCubeShader;
        Window m_Window;
    };
}
