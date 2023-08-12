#include "oakpch.hpp"
#include "Platform/Windows/Window.hpp"

#include "Oak/Core/Input.hpp"

#include "Oak/Events/ApplicationEvent.hpp"
#include "Oak/Events/MouseEvent.hpp"
#include "Oak/Events/KeyEvent.hpp"

#include "Oak/Renderer/Renderer.hpp"

#include "Platform/OpenGL/Context.hpp"

namespace windows
{
    static uint8_t s_GLFWWindowCount = 0;

    static void glfwErrorCallback(int error, const char* description)
    {
        OAK_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window::Window(const oak::WindowProps& props)
    {
        OAK_PROFILE_FUNCTION();

        init(props);
    }

    Window::~Window()
    {
        OAK_PROFILE_FUNCTION();

        shutdown();
    }

    void Window::init(const oak::WindowProps& props)
    {
        OAK_PROFILE_FUNCTION();

        m_Data.title = props.title;
        m_Data.width = props.width;
        m_Data.height = props.height;

        OAK_LOG_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

        if (s_GLFWWindowCount == 0) {
            OAK_PROFILE_SCOPE("glfwInit");
            auto success = glfwInit();
            OAK_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(glfwErrorCallback);
        }

        {
            OAK_PROFILE_SCOPE("glfwCreateWindow");
        #if defined(OAK_DEBUG)
            if (oak::Renderer::getAPI() == oak::RendererAPI::API::OpenGL) {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
            }
        #endif
            m_Window = glfwCreateWindow(static_cast<int>(props.width), static_cast<int>(props.height), m_Data.title.c_str(), nullptr, nullptr);
            ++s_GLFWWindowCount;
        }

        m_Context = oak::GraphicsContext::create(m_Window);
        m_Context->init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
        setVSync(true);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto& data = * reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.width = width;
            data.height = height;

            oak::WindowResizeEvent event(width, height);
            data.eventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
            oak::WindowCloseEvent event;
            data.eventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action) {
            case GLFW_PRESS:
            {
                oak::KeyPressedEvent event(key, 0);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                oak::KeyReleasedEvent event(key);
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                oak::KeyPressedEvent event(key, true);
                data.eventCallback(event);
                break;
            }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
            auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            oak::KeyTypedEvent event(keycode);
            data.eventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS:
            {
                oak::MouseButtonPressedEvent event(button);
                data.eventCallback(event);
                    break;
            }
            case GLFW_RELEASE:
            {
                oak::MouseButtonReleasedEvent event(button);
                data.eventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            oak::MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            data.eventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            auto& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            oak::MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
            data.eventCallback(event);
        });
    }

    void Window::shutdown()
    {
        OAK_PROFILE_FUNCTION();

        glfwDestroyWindow(m_Window);
        --s_GLFWWindowCount;

        if (s_GLFWWindowCount == 0) {
            glfwTerminate();
        }
    }

    void Window::onUpdate()
    {
        OAK_PROFILE_FUNCTION();

        glfwPollEvents();
        m_Context->swapBuffers();
    }

    void Window::setVSync(bool enabled)
    {
        OAK_PROFILE_FUNCTION();

        if (enabled) {
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }

        m_Data.vSync = enabled;
    }

    bool Window::isVSync() const
    {
        return m_Data.vSync;
    }
}
