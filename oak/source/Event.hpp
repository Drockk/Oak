#pragma once
#include <eventpp/eventqueue.h>

namespace oak
{
    enum class EventType
    {
        MouseMoved,
        MouseScrolled,
        WindowClose,
        WindowResize
    };

    class Event
    {
    public:
        explicit Event(const EventType type): m_Type(type)
        {

        }

        virtual ~Event() = default;

        EventType getType() const
        {
            return m_Type;
        }

    private:
        EventType m_Type;
    };

    class MouseMovedEvent: public Event
    {
    public:
        MouseMovedEvent(float x, float y) : m_PosX(x), m_PosY(y), Event(EventType::MouseMoved)
        {

        }

        ~MouseMovedEvent() override = default;

        float getPositionX() const
        {
            return m_PosX;
        }

        float getPositionY() const
        {
            return m_PosY;
        }

    private:
        float m_PosX;
        float m_PosY;
    };

    class MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(float x, float y) : m_OffsetX(x), m_OffsetY(y), Event(EventType::MouseScrolled)
        {

        }

        ~MouseScrolledEvent() override = default;

        float getOffsetX() const
        {
            return m_OffsetX;
        }

        float getOffsetY() const
        {
            return m_OffsetY;
        }

    private:
        float m_OffsetX;
        float m_OffsetY;
    };

    class WindowCloseEvent: public Event
    {
    public:
        WindowCloseEvent(): Event(EventType::WindowClose)
        {

        }

        ~WindowCloseEvent() override = default;
    };

    class WindowResizeEvent: public Event
    {
    public:
        explicit WindowResizeEvent(uint32_t width, uint32_t height): m_Width(width), m_Height(height), Event(EventType::WindowResize)
        {

        }

        ~WindowResizeEvent() override = default;

        uint32_t getWidth() const
        {
            return m_Width;
        }

        uint32_t getHeight() const
        {
            return m_Height;
        }

    private:
        uint32_t m_Width;
        uint32_t m_Height;
    };

    using EventPointer = std::shared_ptr<Event>;

    struct EventPolicy
    {
        static EventType getEvent(const EventPointer& event)
        {
            return event->getType();
        }
    };

    class EventQueue
    {
    public:
        void registerEventListener(EventType, std::function<void(const EventPointer&)> callback);
        void sendEvent(const EventPointer& event);
        void proccess();
    private:
        eventpp::EventQueue<EventType, void(const EventPointer&), EventPolicy> m_EventQueue;
    };
}
