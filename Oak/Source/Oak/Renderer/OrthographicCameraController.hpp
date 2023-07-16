#pragma once

#include "Oak/Renderer/OrthographicCamera.hpp"
#include "Oak/Core/Timestep.hpp"

#include "Oak/Events/ApplicationEvent.hpp"
#include "Oak/Events/MouseEvent.hpp"

namespace oak
{
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float t_aspectRatio, bool t_rotation = false);

        void onUpdate(Timestep t_ts);
        void onEvent(Event& t_e);

        void onResize(float t_width, float t_height);

        OrthographicCamera& getCamera()
        {
            return m_Camera;
        }
        const OrthographicCamera& getCamera() const
        {
            return m_Camera;
        }

        float getZoomLevel() const
        {
            return m_ZoomLevel;
        }
        void setZoomLevel(float t_level)
        {
            m_ZoomLevel = t_level;
        }

    private:
        bool onMouseScrolled(MouseScrolledEvent& t_e);
        bool onWindowResized(WindowResizeEvent& t_e);

        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;

        bool m_Rotation;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
    };
}
