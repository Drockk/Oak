#pragma once

#include "Oak/Events/Event.hpp"
#include "Oak/Core/KeyCodes.hpp"

namespace oak
{
    class KeyEvent : public Event
    {
    public:
        KeyCode getKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(KeyCode t_keycode)
            : m_KeyCode(t_keycode) {}

        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(KeyCode t_keycode, bool t_isRepeat = false)
            : KeyEvent(t_keycode), m_IsRepeat(t_isRepeat) {}

        bool isRepeat() const { return m_IsRepeat; }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(KeyCode t_keycode)
            : KeyEvent(t_keycode) {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(KeyCode t_keycode)
            : KeyEvent(t_keycode) {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}
