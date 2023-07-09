#pragma once

#include <Oak/Core/KeyCodes.hpp>
#include <Oak/Core/MouseCodes.hpp>


#include <glm/glm.hpp>

namespace oak
{
    class Input
    {
    public:
        static bool isKeyPressed(KeyCode t_key);
        static bool isMouseButtonPressed(MouseCode t_button);

        static glm::vec2 getMousePosition();
        static float getMouseX();
        static float getMouseY();
    };
}
