#pragma once
#include <functional>
#include <sstream>

#define BIT(x) (1 << x)

namespace oak
{
    //Events in Oak are currently blocking, meaning when an event occurs it immediately gets dispatched and must be dealt with right then an there.
    //For the future, a better strategy might be to buffer events in an event bus and process them during the "event" part of the update stage.

    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4)
    };

#define EVENT_CLASS_TYPE(type) \
    static EventType getStaticType() { return EventType::type; } \
    EventType getEventType() const override { return getStaticType(); } \
    std::string_view getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) int getCategoryFlags() const override { return category; }

    class Event
    {
    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType getEventType() const = 0;
        [[nodiscard]] virtual std::string_view getName() const = 0;
        [[nodiscard]] virtual int getCategoryFlags() const = 0;
        [[nodiscard]] virtual std::string toString() const { return std::string(getName()); }

        [[nodiscard]] bool isInCategory(EventCategory category) const
        {
            return getCategoryFlags() & category;
        }

        [[nodiscard]] bool getHandled() const
        {
            return m_Handled;
        }

        void setHandled(bool t_handled)
        {
            m_Handled = t_handled;
        }

    private:
        bool m_Handled{false};
    };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& t_event) : m_Event(t_event) {}

        //F will be deduced by the compiler
        template<typename T, typename F>
        bool dispatch(const F& t_func)
        {
            if (m_Event.getEventType() == T::getStaticType())
            {
                m_Event.getHandled() |= t_func(static_cast<T&>(m_Event));
                return true;
            }

            return false;
        }

    private:
        Event& m_Event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e) {
        return os << e.toString();
    }
}

