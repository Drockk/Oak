#pragma once

#include "Oak/Core/KeyCodes.hpp"
#include "Oak/Core/MouseCodes.hpp"

#include <glm/glm.hpp>

namespace oak {
    class Input
    {
    public:
        static bool isKeyPressed(oak::KeyCode key);

        static bool isMouseButtonPressed(oak::MouseCode button);
        static glm::vec2 getMousePosition();
        static float getMouseX();
        static float getMouseY();
    };
}
