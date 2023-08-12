#pragma once

#include "Oak/Core/Base.hpp"
#include "Oak/Events/Event.hpp"

#include <sstream>

namespace oak {
    struct WindowProps
    {
        std::string title;
        uint32_t width;
        uint32_t height;

        WindowProps(const std::string& t_title = "Oak Engine", uint32_t t_width = 1600, uint32_t t_height = 900): title(t_title), width(t_width), height(t_height)
        {
        }
    };

    // Interface representing a desktop system based Window
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void onUpdate() = 0;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

        // Window attributes
        virtual void setEventCallback(const EventCallbackFn& callback) = 0;
        virtual void setVSync(bool enabled) = 0;
        virtual bool isVSync() const = 0;

        virtual void* getNativeWindow() const = 0;

        static Scope<Window> create(const WindowProps& props = WindowProps());
    };
}
