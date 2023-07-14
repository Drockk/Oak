#pragma once

#include "Oak/Core/Window.hpp"
#include "Oak/Renderer/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

namespace windows
{
    class Window : public oak::Window
    {
    public:
        Window(const oak::WindowProps& t_props);
        ~Window() override;

        void onUpdate() override;

        std::pair<uint32_t, uint32_t> getResolution() const override
        {
            return m_Data.resolution;
        }

        // Window attributes
        void setEventCallback(const EventCallbackFn& t_callback) override { m_Data.eventCallback = t_callback; }
        void setVSync(bool t_enabled) override;
        bool isVSync() const override;

        void* getNativeWindow() const { return m_Window; }
    private:
        virtual void init(const oak::WindowProps& t_props);
        virtual void shutdown();
    private:
        GLFWwindow* m_Window;
        oak::Scope<oak::GraphicsContext> m_Context;

        struct WindowData
        {
            std::string title;
            std::pair<uint32_t, uint32_t> resolution;
            bool vSync;

            EventCallbackFn eventCallback;
        };

        WindowData m_Data;
    };
}
