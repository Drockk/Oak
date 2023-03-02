#include "Window.hpp"

#include <iostream>

namespace oak
{
    WindowErrorCodes Window::createWindow(const std::string& title, uint32_t width, uint32_t height, std::shared_ptr<EventQueue> eventQueue)
    {
        m_WindowProperties = { title, width, height, eventQueue };

        if (!glfwInit())
        {
            std::cerr << "Cannot initialize GLFW\n";
            return WindowErrorCodes::CANNOT_INITIALIZE_GLFW;
        }

        //Create a windowed mode window and its OpenGL context.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            glfwTerminate();
            return WindowErrorCodes::CANNOT_CREATE_WINDOW;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, reinterpret_cast<void*>(&m_WindowProperties));

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y) {
            auto properties = reinterpret_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
            properties->eventQueue->sendEvent(std::make_shared<MouseMovedEvent>(static_cast<float>(x), static_cast<float>(y)));
        });

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto properties = reinterpret_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
            properties->eventQueue->sendEvent(std::make_shared<WindowResizeEvent>(width, height));
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double x, double y) {
            auto properties = reinterpret_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
            properties->eventQueue->sendEvent(std::make_shared<MouseScrolledEvent>(x, y));
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            auto properties = reinterpret_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
            properties->eventQueue->sendEvent(std::make_shared<WindowCloseEvent>());
        });
    }

    void Window::onUpdate()
    {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    void Window::onShutdown()
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
        glfwTerminate();
    }
}
