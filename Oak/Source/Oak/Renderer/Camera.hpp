#pragma once
#include <glm/glm.hpp>

namespace oak
{
    class Camera
    {
    public:
        Camera() = default;
        Camera(const glm::mat4& t_projection) : m_Projection{ t_projection }
        {

        }
        virtual ~Camera() = default;

        const glm::mat4& getProjection() const
        {
            return m_Projection;
        }

    protected:
        glm::mat4 m_Projection{ glm::mat4(1.0f) };
    };
}
