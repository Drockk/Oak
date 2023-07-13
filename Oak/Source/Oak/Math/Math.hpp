#pragma once

#include <glm/glm.hpp>

namespace oak::math
{
    bool decomposeTransform(const glm::mat4& t_transform, glm::vec3& t_translation, glm::vec3& t_rotation, glm::vec3& t_scale);
}
