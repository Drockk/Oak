#pragma once
#include "Event.hpp"

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

    struct WindowProperties
    {
        std::string title;
        uint32_t width;
        uint32_t height;
        std::shared_ptr<EventQueue> eventQueue;
    };

    class Window
    {
    public:
        Window() = default;
        ~Window() = default;

        WindowErrorCodes createWindow(const std::string& title, uint32_t width, uint32_t height, std::shared_ptr<EventQueue> eventQueue);

        void onUpdate();
        void onShutdown();

    private:
        GLFWwindow* m_Window{ nullptr };
        WindowProperties m_WindowProperties;
    };
}
