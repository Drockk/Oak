#pragma once

#include "Oak/Core/Window.hpp"
#include "Oak/Renderer/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

namespace windows
{
    class Window final : public oak::Window
    {
    public:
        Window(const oak::WindowProps& props);
        ~Window() override;

        void onUpdate() override;

        unsigned int getWidth() const override { return m_Data.width; }
        unsigned int getHeight() const override { return m_Data.height; }

        // Window attributes
        void setEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }
        void setVSync(bool enabled) override;
        bool isVSync() const override;

        void* getNativeWindow() const override { return m_Window; }

    private:
        void init(const oak::WindowProps& props);
        void shutdown();

        GLFWwindow* m_Window;
        oak::Scope<oak::GraphicsContext> m_Context;

        struct WindowData
        {
            std::string title;
            unsigned int width, height;
            bool vSync;

            EventCallbackFn eventCallback;
        };

        WindowData m_Data;
    };
}
