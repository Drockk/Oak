#include "oakpch.hpp"
#include "Platform/Windows/Window.hpp"

#include <stdexcept>

namespace Windows
{
    Window::Window(const std::string& name, std::pair<uint32_t, uint32_t> resolution)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Cannot initialize GLFW");
        }

        auto [width, height] = resolution;
        m_Window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            glfwTerminate();
            throw std::runtime_error("Cannot create GLFW window");
        }

        glfwMakeContextCurrent(m_Window);
    }

    Window::~Window()
    {
        glfwTerminate();
    }

    void Window::run()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();
        }
    }
}
