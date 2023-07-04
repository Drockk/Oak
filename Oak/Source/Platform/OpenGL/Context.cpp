#include "oakpch.hpp"

#include "Platform/OpenGL/Context.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <stdexcept>

namespace openGL
{
    Context::Context(std::any t_windowHandle)
    {
        if (!t_windowHandle.has_value()) {
            throw std::invalid_argument("Window handle is empty");
        }

        auto windowHandle = std::any_cast<GLFWwindow*>(t_windowHandle);
        if (windowHandle == nullptr) {
            throw std::invalid_argument("Window handle is null");
        }

        m_WindowHandle = static_cast<GLFWwindow*>(windowHandle);
    }

    void Context::init()
    {
        glfwMakeContextCurrent(m_WindowHandle);

        const auto version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            throw std::runtime_error("Failed to initialize OpenGL Context");
        }

        OAK_CORE_INFO("OpenGL Info:");
        OAK_CORE_INFO("  Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        OAK_CORE_INFO("  Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        OAK_CORE_INFO("  Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        const auto majorVersion = GLAD_VERSION_MAJOR(version);
        const auto minorVersion = GLAD_VERSION_MINOR(version);
        if (majorVersion < 4 or (majorVersion == 4 and minorVersion < 6)) {
            throw std::runtime_error("Oak requires at least OpenGL version 4.6!");
        }
    }

    void Context::swapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}
