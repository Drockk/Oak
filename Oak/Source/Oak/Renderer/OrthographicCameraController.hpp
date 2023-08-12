#pragma once

#include "Oak/Renderer/OrthographicCamera.hpp"
#include "Oak/Core/Timestep.hpp"

#include "Oak/Events/ApplicationEvent.hpp"
#include "Oak/Events/MouseEvent.hpp"

namespace oak {
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);

        void onUpdate(Timestep ts);
        void onEvent(Event& e);

        void onResize(float width, float height);

        OrthographicCamera& getCamera() { return m_Camera; }
        const OrthographicCamera& getCamera() const { return m_Camera; }

        float getZoomLevel() const { return m_ZoomLevel; }
        void setZoomLevel(float level) { m_ZoomLevel = level; }
    private:
        bool onMouseScrolled(MouseScrolledEvent& e);
        bool onWindowResized(WindowResizeEvent& e);
    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;

        bool m_Rotation;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
    };
}
