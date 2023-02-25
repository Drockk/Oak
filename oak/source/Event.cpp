#include "Event.hpp"

namespace oak
{
    void EventQueue::registerEventListener(EventType type, std::function<void(const EventPointer&)> callback)
    {
        m_EventQueue.appendListener(type, callback);
    }

    void EventQueue::sendEvent(const EventPointer& event)
    {
        m_EventQueue.enqueue(event);
    }

    void EventQueue::proccess()
    {
        m_EventQueue.process();
    }
}
