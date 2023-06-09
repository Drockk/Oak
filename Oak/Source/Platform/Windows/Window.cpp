#include "oakpch.hpp"
#include "Platform/Windows/Window.hpp"

#include <stdexcept>

namespace Windows
{
    Window::Window(const std::string& name, std::pair<uint32_t, uint32_t> resolution)
    {
        if (s_WindowCount <= 0)
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

        createWindow(name, resolution);

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

    void Window::createWindow(const std::string& name, std::pair<uint32_t, uint32_t> resolution)
    {
        auto [width, height] = resolution;
        m_Window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            glfwTerminate();
            throw std::runtime_error("Cannot create GLFW window");
        }
    }

    void Window::setCallbacks()
    {
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            OAK_CORE_CRITICAL("TODO Implement WindowsCloseCallback");
            });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            OAK_CORE_CRITICAL("TODO Implement WindowSizeCallback");
            OAK_CORE_TRACE("Width: {} Height: {}", width, height);
            });

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            OAK_CORE_CRITICAL("TODO Implement FramebufferSizeCallback");
            OAK_CORE_TRACE("Width: {} Height: {}", width, height);
            });

        glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow* window, float xscale, float yscale) {
            OAK_CORE_CRITICAL("TODO Implement WindowContentScaleCallback");
            OAK_CORE_TRACE("XScale: {} YScale: {}", xscale, yscale);
            });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos) {
            OAK_CORE_CRITICAL("TODO Implement WindowPosCallback");
            OAK_CORE_TRACE("XPos: {} YPos: {}", xpos, ypos);
            });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
            OAK_CORE_CRITICAL("TODO Implement WindowFocusCallback");
            OAK_CORE_TRACE("focused: {}", focused);
            });
    }
}
