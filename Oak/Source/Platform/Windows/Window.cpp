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

        m_Context = oak::GraphicsContext::create(
            { m_Window });
        m_Context->init();
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

    void Window::onUpdate()
    {
        m_Context->swapBuffers();
        glfwPollEvents();
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

            oak::WindowCloseEvent event;
            data->onEvent(event);
            });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto data = reinterpret_cast<oak::WindowData*>(glfwGetWindowUserPointer(window));

            if (!data->onEvent)
            {
                throw std::invalid_argument("onEvent callback not setted");
            }

            oak::WindowResizeEvent event(width, height);
            data->onEvent(event);
            });

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto data = reinterpret_cast<oak::WindowData*>(glfwGetWindowUserPointer(window));

            if (!data->onEvent)
            {
                throw std::invalid_argument("onEvent callback not setted");
            }

            oak::WindowResizeEvent event(width, height);
            data->onEvent(event);
            });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
            auto data = reinterpret_cast<oak::WindowData*>(glfwGetWindowUserPointer(window));

            if (!data->onEvent)
            {
                throw std::invalid_argument("onEvent callback not setted");
            }

            oak::WindowFocusEvent event(focused);
            data->onEvent(event);
            });
    }
}
