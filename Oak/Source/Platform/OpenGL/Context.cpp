#include "oakpch.hpp"
#include "Platform/OpenGL/Context.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace opengl {
    Context::Context(GLFWwindow* t_windowHandle): m_WindowHandle(t_windowHandle)
    {
        if (!t_windowHandle) {
            throw std::invalid_argument("Window handle is null!");
        }
    }

    auto Context::init() -> void
    {
        OAK_PROFILE_FUNCTION();

        glfwMakeContextCurrent(m_WindowHandle);
        const auto version = gladLoadGL(glfwGetProcAddress);
        if (!version) {
            throw std::invalid_argument("Failed to initialize Glad!");
        }

        OAK_LOG_CORE_INFO("OpenGL Info:");
        OAK_LOG_CORE_INFO("  Vendor:    {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        OAK_LOG_CORE_INFO("  Renderer:  {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        OAK_LOG_CORE_INFO("  Version:   {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        const auto major = GLAD_VERSION_MAJOR(version);
        const auto minor = GLAD_VERSION_MINOR(version);
        if (major < 4 || (major != 4 && minor < 5)) {
            throw std::invalid_argument("Oak requires at least OpenGL version 4.5!");
        }
    }

    auto Context::swapBuffers() -> void
    {
        OAK_PROFILE_FUNCTION();
        glfwSwapBuffers(m_WindowHandle);
    }
}
