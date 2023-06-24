#pragma once
#include "Oak/Core/Window.hpp"

#include <GLFW/glfw3.h>

#include <utility>

namespace Windows
{
    class Window final : public oak::Window
    {
    public:
        Window(oak::WindowData t_data);
        ~Window() override;

        [[noreturn]] void run() override;

    private:
        [[noreturn]] void createWindow(oak::WindowData t_data);
        [[noreturn]] void setCallbacks();

        GLFWwindow* m_Window{nullptr};
        inline static uint8_t s_WindowCount{ 0 };
    };
}
