#include "oakpch.hpp"
#include "Platform/Windows/Window.hpp"

#include "Oak/Core/Input.hpp"

#include "Oak/Events/ApplicationEvent.hpp"
#include "Oak/Events/MouseEvent.hpp"
#include "Oak/Events/KeyEvent.hpp"

#include "Oak/Renderer/Renderer.hpp"

#include "Platform/OpenGL/OpenGLContext.hpp"

namespace windows
{
    static uint8_t s_GLFWWindowCount{ 0 };

    static void GLFWErrorCallback(int t_error, const char* t_description)
    {
        OAK_LOG_CORE_ERROR("GLFW Error ({0}): {1}", t_error, t_description);
    }

    Window::Window(const oak::WindowProps& t_props)
    {
        OAK_PROFILE_FUNCTION();

        init(t_props);
    }

    Window::~Window()
    {
        OAK_PROFILE_FUNCTION();

        shutdown();
    }

    void Window::init(const oak::WindowProps& t_props)
    {
        OAK_PROFILE_FUNCTION();

        m_Data.title = t_props.title;
        m_Data.resolution = t_props.resolution;

        auto [width, height] = t_props.resolution;
        OAK_LOG_CORE_INFO("Creating window {0} ({1}, {2})", t_props.title, width, height);

        if (s_GLFWWindowCount == 0)
        {
            OAK_PROFILE_SCOPE("glfwInit");
            auto success = glfwInit();
            OAK_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        {
            OAK_PROFILE_SCOPE("glfwCreateWindow");
#if defined(OAK_DEBUG)
            if (Renderer::getAPI() == RendererAPI::API::OpenGL)
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
            }
#endif
            m_Window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), m_Data.title.c_str(), nullptr, nullptr);
            ++s_GLFWWindowCount;
        }

        m_Context = GraphicsContext::create(m_Window);
        m_Context->init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
        setVSync(true);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* t_window, int t_width, int t_height)
            {
                auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(t_window));
                data.resolution = { t_width, t_height };

                oak::WindowResizeEvent event({ t_width, t_height });
                data.eventCallback(event);
            });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* t_window)
            {
                auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(t_window));
                oak::WindowCloseEvent event;
                data.eventCallback(event);
            });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* t_window, int t_key, int, int t_action, int)
            {
                auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(t_window));

                switch (t_action)
                {
                case GLFW_PRESS:
                {
                    oak::KeyPressedEvent event(t_key, 0);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    oak::KeyReleasedEvent event(t_key);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    oak::KeyPressedEvent event(t_key, true);
                    data.eventCallback(event);
                    break;
                }
                }
            });

        glfwSetCharCallback(m_Window, [](GLFWwindow* t_window, unsigned int t_keycode)
            {
                auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(t_window));

                oak::KeyTypedEvent event(t_keycode);
                data.eventCallback(event);
            });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* t_window, int t_button, int t_action, int)
            {
                auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(t_window));

                switch (t_action)
                {
                case GLFW_PRESS:
                {
                    oak::MouseButtonPressedEvent event(t_button);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    oak::MouseButtonReleasedEvent event(t_button);
                    data.eventCallback(event);
                    break;
                }
                }
            });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* t_window, double t_xOffset, double t_yOffset)
            {
                auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(t_window));

                oak::MouseScrolledEvent event({ static_cast<float>(t_xOffset), static_cast<float>(t_yOffset) });
                data.eventCallback(event);
            });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* t_window, double t_xPos, double t_yPos)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(t_window);

                oak::MouseMovedEvent event({ static_cast<float>(t_xPos), static_cast<float>(t_yPos) });
                data.eventCallback(event);
            });
    }

    void Window::shutdown()
    {
        OAK_PROFILE_FUNCTION();

        glfwDestroyWindow(m_Window);
        --s_GLFWWindowCount;

        if (s_GLFWWindowCount == 0)
        {
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

        if (enabled)
        {
            glfwSwapInterval(1);
        }
        else
        {
            glfwSwapInterval(0);
        }

        m_Data.vSync = enabled;
    }

    bool Window::isVSync() const
    {
        return m_Data.vSync;
    }
}
