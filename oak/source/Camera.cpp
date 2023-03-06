#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace oak
{
    Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float fov, float yaw, float pitch, uint32_t width, uint32_t height): m_Position(position), m_Front(front), m_Up(up), m_Fov(fov), m_Yaw(yaw), m_Pitch(pitch), m_Width(width), m_Height(height)
    {
        recalculateProjection();
    }

    void Camera::recalculateProjection()
    {
        m_Projection = glm::perspective(glm::radians(m_Fov), static_cast<float>(m_Width) / static_cast<float>(m_Height), 0.1f, 100.0f);
    }

    void Camera::registerEvents(std::shared_ptr<EventQueue> eventQueue)
    {
        eventQueue->registerEventListener(EventType::MouseMoved, [this](const EventPointer& event) {
            auto mouseMovedEvent = static_cast<const MouseMovedEvent*>(event.get());
            auto xPos = mouseMovedEvent->getPositionX();
            auto yPos = mouseMovedEvent->getPositionY();

            if (m_FirstMouse)
            {
                m_LastX = xPos;
                m_LastY = yPos;
                m_FirstMouse = false;
            }

            auto xoffset = xPos - m_LastX;
            auto yoffset = m_LastY - yPos; // reversed since y-coordinates go from bottom to top
            m_LastX = xPos;
            m_LastY = yPos;

            auto sensitivity{ 0.1f };
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            m_Yaw += xoffset;
            m_Pitch += yoffset;

            if (m_Pitch > 89.0f)
            {
                m_Pitch = 89.0f;
            }
            if (m_Pitch < -89.0f)
            {
                m_Pitch = -89.0f;
            }

            glm::vec3 front{};
            front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            front.y = sin(glm::radians(m_Pitch));
            front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            m_Front = glm::normalize(front);
        });

        eventQueue->registerEventListener(EventType::MouseScrolled, [this](const EventPointer& event) {
            auto mouseScrolledEvent = static_cast<const MouseScrolledEvent*>(event.get());
            auto offsetY = mouseScrolledEvent->getOffsetY();

            m_Fov -= offsetY;
            if (m_Fov < 1.0f)
            {
                m_Fov = 1.0f;
            }
            if (m_Fov > 45.0f)
            {
                m_Fov = 45.0f;
            }

            recalculateProjection();
        });
    }

    void Camera::onUpdate()
    {
        recalculateView();
    }

    void Camera::recalculateView()
    {
        m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }
}
