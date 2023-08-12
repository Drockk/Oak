#include "oakpch.hpp"
#include "Platform/OpenGL/Context.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace opengl {
    Context::Context(GLFWwindow* windowHandle): m_WindowHandle(windowHandle)
    {
        OAK_CORE_ASSERT(windowHandle, "Window handle is null!")
    }

    void Context::init()
    {
        OAK_PROFILE_FUNCTION();

        glfwMakeContextCurrent(m_WindowHandle);
        int version = gladLoadGL(glfwGetProcAddress);
        OAK_CORE_ASSERT(version, "Failed to initialize Glad!");

        OAK_LOG_CORE_INFO("OpenGL Info:");
        //FIX ME:
        //OAK_LOG_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        //OAK_LOG_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        //OAK_LOG_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

        auto major{ GLAD_VERSION_MAJOR(version) };
        auto minor{ GLAD_VERSION_MINOR(version) };
        OAK_CORE_ASSERT(major > 4 || (major == 4 && minor >= 5), "Oak requires at least OpenGL version 4.5!");
    }

    void Context::swapBuffers()
    {
        OAK_PROFILE_FUNCTION();

        glfwSwapBuffers(m_WindowHandle);
    }
}
