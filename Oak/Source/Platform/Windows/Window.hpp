#pragma once
#include <GLFW

#include <memory>

namespace Windows
{
    class Window
    {
    public:
        Window();
        ~Window();

    private:
        std::unique_ptr<GLFWwindow> m_Window;
    };
}
