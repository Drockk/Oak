#include "oakpch.hpp"
#include "Oak/Renderer/OrthographicCameraController.hpp"

#include "Oak/Core/Input.hpp"
#include "Oak/Core/KeyCodes.hpp"

namespace oak
{
    OrthographicCameraController::OrthographicCameraController(float t_aspectRatio, bool t_rotation): m_AspectRatio(t_aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(t_rotation)
    {
    }

    void OrthographicCameraController::onUpdate(Timestep t_ts)
    {
        OAK_PROFILE_FUNCTION();

        if (Input::isKeyPressed(Key::A))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
        }
        else if (Input::isKeyPressed(Key::D))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
        }

        if (Input::isKeyPressed(Key::W))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
        }
        else if (Input::isKeyPressed(Key::S))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * t_ts;
        }

        if (m_Rotation)
        {
            if (Input::isKeyPressed(Key::Q))
                m_CameraRotation += m_CameraRotationSpeed * t_ts;
            if (Input::isKeyPressed(Key::E))
                m_CameraRotation -= m_CameraRotationSpeed * t_ts;

            if (m_CameraRotation > 180.0f)
                m_CameraRotation -= 360.0f;
            else if (m_CameraRotation <= -180.0f)
                m_CameraRotation += 360.0f;

            m_Camera.setRotation(m_CameraRotation);
        }

        m_Camera.setPosition(m_CameraPosition);

        m_CameraTranslationSpeed = m_ZoomLevel;
    }

    void OrthographicCameraController::onEvent(Event& t_e)
    {
        OAK_PROFILE_FUNCTION();

        EventDispatcher dispatcher(t_e);
        dispatcher.dispatch<MouseScrolledEvent>(OAK_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolled));
        dispatcher.dispatch<WindowResizeEvent>(OAK_BIND_EVENT_FN(OrthographicCameraController::onWindowResized));
    }

    void OrthographicCameraController::onResize(float t_width, float t_height)
    {
        m_AspectRatio = t_width / t_height;
        m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    }

    bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& t_e)
    {
        OAK_PROFILE_FUNCTION();

        auto [x, y] = t_e.getOffset();
        m_ZoomLevel -= y * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    bool OrthographicCameraController::onWindowResized(WindowResizeEvent& t_e)
    {
        OAK_PROFILE_FUNCTION();

        auto [width, height] = t_e.getResolution();
        onResize((float)width, (float)height);
        return false;
    }
}
