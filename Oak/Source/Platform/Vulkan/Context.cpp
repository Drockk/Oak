#include "oakpch.hpp"
#include "Platform/Vulkan/Context.hpp"

#include <GLFW/glfw3.h>

namespace vulkan {
    Context::Context(GLFWwindow* t_windowHandle) : m_WindowHandle(t_windowHandle)
    {
        if (!t_windowHandle) {
            throw std::invalid_argument("Window handle is null!");
        }
    }

    auto Context::init() -> void
    {
        OAK_PROFILE_FUNCTION();
        throw std::runtime_error("Not implemented");
    }

    auto Context::swapBuffers() -> void
    {
        OAK_PROFILE_FUNCTION();
        throw std::runtime_error("Not implemented");
    }
}
