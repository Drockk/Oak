#include "oakpch.hpp"
#include "Platform/Windows/Window.hpp"

#include <stdexcept>

namespace Windows
{
    Window::Window()
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Cannot initialize GLFW");
        }
    }

    Window::~Window()
    {
        glfwTerminate();
    }
}
