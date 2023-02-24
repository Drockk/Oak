#pragma once
#include <GLFW/glfw3.h>

#include <string>

namespace oak
{
    enum class WindowErrorCodes
    {
        SUCCESS,
        CANNOT_INITIALIZE_GLFW,
        CANNOT_CREATE_WINDOW
    };

    class Window
    {
    public:
        Window() = default;
        ~Window() = default;

        WindowErrorCodes createWindow(const std::string& title, uint32_t width, uint32_t height);

    private:
        GLFWwindow* m_Window{ nullptr };
    };
}
