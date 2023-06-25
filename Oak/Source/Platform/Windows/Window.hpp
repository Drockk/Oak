#pragma once
#include "Oak/Core/Window.hpp"
#include "Oak/Renderer/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

#include <utility>

namespace Windows
{
    class Window final : public oak::Window
    {
    public:
        Window(oak::WindowData t_data);
        ~Window() override;

        void onUpdate() override;

    private:
        void createWindow(oak::WindowData t_data);
        void setCallbacks();

        GLFWwindow* m_Window{nullptr};
        inline static uint8_t s_WindowCount{ 0 };

        std::shared_ptr<oak::GraphicsContext> m_Context;
    };
}
