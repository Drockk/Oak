#pragma once

#include "Oak/Events/Event.hpp"
#include "Oak/Core/MouseCodes.hpp"

namespace oak
{
    class MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(std::pair<float, float> t_position)
            : m_Position{ t_position } {}

        std::pair<float, float> getPosition() const
        {
            return m_Position;
        }

        std::string toString() const override
        {
            auto [x, y] = m_Position;
            std::stringstream ss;
            ss << "MouseMovedEvent: " << x << ", " << y;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        std::pair<float, float> m_Position{};
    };

    class MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(std::pair<float, float> t_offset)
            : m_Offset{ t_offset } {}

        std::pair<float, float> getOffset() const
        {
            return m_Offset;
        }

        std::string toString() const override
        {
            std::stringstream ss;
            auto [x, y] = m_Offset;
            ss << "MouseScrolledEvent: " << x << ", " << y;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        std::pair<float, float> m_Offset{};
    };

    class MouseButtonEvent : public Event
    {
    public:
        MouseCode getMouseButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
    protected:
        MouseButtonEvent(const MouseCode t_button)
            : m_Button(t_button) {}

        MouseCode m_Button;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(MouseCode t_button)
            : MouseButtonEvent(t_button) {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(MouseCode t_button)
            : MouseButtonEvent(t_button) {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}
