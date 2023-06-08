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

        void run() override;
    private:
        GLFWwindow* m_Window;
    };
}
