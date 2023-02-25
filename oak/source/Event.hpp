#pragma once
#include <eventpp/eventqueue.h>

namespace oak
{
    enum class EventType
    {
        WindowClose
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

    class WindowCloseEvent: public Event
    {
    public:
        WindowCloseEvent() : Event(EventType::WindowClose)
        {

        }
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
