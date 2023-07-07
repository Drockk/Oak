#pragma once

#include "Oak/Events/Event.hpp"

namespace oak
{
    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(std::pair<uint32_t, uint32_t> t_resolution)
            : m_Resolution{ t_resolution } {}

        std::pair<uint32_t, uint32_t> getResolution() const { return m_Resolution; }

        std::string toString() const override
        {
            auto [width, height] = m_Resolution;
            std::stringstream ss;
            ss << "WindowResizeEvent: " << width << ", " << height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    private:
        std::pair<uint32_t, uint32_t> m_Resolution;
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppTickEvent : public Event
    {
    public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent : public Event
    {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent : public Event
    {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}
