#pragma once
#include <Event.hpp>

#include <glm/glm.hpp>

namespace oak
{
    class Camera
    {
    public:
        Camera() = default;
        Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float fov, float yaw, float pitch, uint32_t width, uint32_t height);
        ~Camera() = default;

        void recalculateProjection();
        void registerEvents(std::shared_ptr<EventQueue> eventQueue);

        void onUpdate();

        glm::mat4 getProjection() const
        {
            return m_Projection;
        }

        glm::mat4 getView() const
        {
            return m_View;
        }

    private:
        void recalculateView();

        bool m_FirstMouse{ false };

        uint32_t m_Width;
        uint32_t m_Height;

        float m_Fov;
        float m_LastX;
        float m_LastY;
        float m_Pitch;
        float m_Yaw;

        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Up;

        glm::mat4 m_Projection;
        glm::mat4 m_View;
    };
}
