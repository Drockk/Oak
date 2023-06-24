#include "oakpch.hpp"
#include "Platform/Windows/Window.hpp"

#include"Oak/Events/ApplicationEvent.hpp"

#include <stdexcept>

namespace Windows
{
    Window::Window(oak::WindowData t_data): oak::Window(t_data)
    {
        if (s_WindowCount == 0)
        {
            if (!glfwInit())
            {
                throw std::runtime_error("Cannot initialize GLFW");
            }

            int major{}, minor{}, revision{};
            glfwGetVersion(&major, &minor, &revision);
            OAK_CORE_INFO("GLFW version: {}.{}.{}", major, minor, revision);
        }

        glfwSetErrorCallback([](int code, const char* description) {
            throw std::runtime_error(std::format("GLFW Error({}): {}", code, description));
            });

        createWindow(t_data);

        glfwMakeContextCurrent(m_Window);

        setCallbacks();

        ++s_WindowCount;
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
        --s_WindowCount;

        if (s_WindowCount == 0)
        {
            glfwTerminate();
        }
    }

    void Window::run()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            glfwSwapBuffers(m_Window);
            glfwPollEvents();
        }
    }

    void Window::createWindow(oak::WindowData t_data)
    {
        auto [width, height] = t_data.resolution;
        m_Window = glfwCreateWindow(width, height, t_data.name.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            glfwTerminate();
            throw std::runtime_error("Cannot create GLFW window");
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);
    }

    void Window::setCallbacks()
    {
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            auto data = reinterpret_cast<oak::WindowData*>(glfwGetWindowUserPointer(window));

            if (!data->onEvent)
            {
                throw std::invalid_argument("onEvent callback not setted");
            }

            data->onEvent(oak::WindowCloseEvent());
            });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            OAK_CORE_CRITICAL("TODO Implement WindowSizeCallback");
            OAK_CORE_TRACE("Width: {} Height: {}", width, height);
            });

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto data = reinterpret_cast<oak::WindowData*>(glfwGetWindowUserPointer(window));

            if (!data->onEvent)
            {
                throw std::invalid_argument("onEvent callback not setted");
            }

            data->onEvent(oak::WindowResizeEvent(width, height));
            });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
            auto data = reinterpret_cast<oak::WindowData*>(glfwGetWindowUserPointer(window));

            if (!data->onEvent)
            {
                throw std::invalid_argument("onEvent callback not setted");
            }

            data->onEvent(oak::WindowFocusEvent(focused));
            });
    }
}
