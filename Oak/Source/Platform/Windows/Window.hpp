#pragma once
#include "Oak/Core/Window.hpp"

#include <GLFW/glfw3.h>

#include <utility>

namespace Windows
{
    class Window final : public oak::Window
    {
    public:
        Window(const std::string& name, std::pair<uint32_t, uint32_t> resolution);
        ~Window() override;

        [[noreturn]] void run() override;

    private:
        [[noreturn]] void createWindow(const std::string& name, std::pair<uint32_t, uint32_t> resolution);
        [[noreturn]] void setCallbacks();

        GLFWwindow* m_Window{nullptr};
        inline static uint8_t s_WindowCount{ 0 };
    };
}
