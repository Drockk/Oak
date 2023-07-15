#pragma once

#include "Oak/Core/Timestep.hpp"
#include "Oak/Events/Event.hpp"
#include "Oak/Events/MouseEvent.hpp"
#include "Oak/Renderer/Camera.hpp"

#include <glm/glm.hpp>

namespace oak
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float t_fov, float t_aspectRatio, float t_nearClip, float t_farClip);
        ~EditorCamera() override = default;

        void onUpdate(Timestep t_timestep);
        void onEvent(Event& t_event);

        inline float getDistance() const
        {
            return m_Distance;
        }

        void setDistance(float t_distance)
        {
            m_Distance = t_distance;
        }

        inline void setViewportSize(std::pair<float, float> t_size)
        {
            m_ViewportSize = t_size;
            updateProjection();
        }

        const glm::mat4& getViewMatrix() const
        {
            return m_ViewMatrix;
        }

        glm::mat4 getViewProjection() const
        {
            return m_Projection * m_ViewMatrix;
        }

        glm::vec3 getUpDirection() const;
        glm::vec3 getRightDirection() const;
        glm::vec3 getForwardDirection() const;
        const glm::vec3& getPosition() const
        {
            return m_Position;
        }
        glm::quat getOrientation() const;

        float getPitch() const
        {
            return m_Pitch;
        }
        float getYaw() const
        {
            return m_Yaw;
        }

    private:
        void updateProjection();
        void updateView();

        bool onMouseScroll(MouseScrolledEvent& t_event);

        void mousePan(const glm::vec2& t_delta);
        void mouseRotate(const glm::vec2& t_delta);
        void mouseZoom(float t_delta);

        glm::vec3 calculatePosition() const;

        std::pair<float, float> panSpeed() const;
        float rotationSpeed() const;
        float zoomSpeed() const;

        float m_FOV{ 45.0f };
        float m_AspectRatio{ 1.778f };
        float m_NearClip{ 0.1f };
        float m_FarClip{ 1000.0f };

        glm::mat4 m_ViewMatrix{};
        glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 m_FocalPoint{ 0.0f, 0.0f, 0.0f };

        glm::vec2 m_InitialMousePosition{ 0.0f, 0.0f };

        float m_Distance{ 10.0f };
        float m_Pitch{ 0.0f };
        float m_Yaw{ 0.0f };

        std::pair<float, float> m_ViewportSize{ 1280.0f, 720.0f };
    };
}
