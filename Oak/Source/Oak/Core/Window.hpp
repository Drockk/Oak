#pragma once

#include "Oak/Core/Base.hpp"
#include "Oak/Events/Event.hpp"

#include <sstream>

namespace oak
{
    struct WindowProps
    {
        std::string title;
        std::pair<uint32_t, uint32_t> resolution;

        WindowProps(const std::string& t_title = "Oak Engine", std::pair<uint32_t, uint32_t> t_resolution = {1600, 900})
            : title{ t_title }, resolution{ t_resolution }
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

        virtual std::pair<uint32_t, uint32_t> getResolution() const = 0;

        // Window attributes
        virtual void setEventCallback(const EventCallbackFn& t_callback) = 0;
        virtual void setVSync(bool t_enabled) = 0;
        virtual bool isVSync() const = 0;

        virtual void* getNativeWindow() const = 0;

        static Scope<Window> create(const WindowProps& t_props = WindowProps());
    };
}
