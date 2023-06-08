#pragma once
#include <GLFW/glfw3.h>

namespace Windows
{
    class Window
    {
    public:
        Window();
        ~Window();

    private:
        GLFWwindow* m_Window;
    };
}
