#include "oakpch.hpp"
#include "Oak/Renderer/OrthographicCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace oak
{
    OrthographicCamera::OrthographicCamera(float t_left, float t_right, float t_bottom, float t_top)
        : m_ProjectionMatrix(glm::ortho(t_left, t_right, t_bottom, t_top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
    {
        OAK_PROFILE_FUNCTION();

        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::setProjection(float t_left, float t_right, float t_bottom, float t_top)
    {
        OAK_PROFILE_FUNCTION();

        m_ProjectionMatrix = glm::ortho(t_left, t_right, t_bottom, t_top, -1.0f, 1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::recalculateViewMatrix()
    {
        OAK_PROFILE_FUNCTION();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
