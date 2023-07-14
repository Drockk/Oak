#include "oakpch.hpp"
#include "Oak/Core/Application.hpp"
#include "Oak/Core/Input.hpp"

#include <GLFW/glfw3.h>

namespace oak
{
    bool Input::isKeyPressed(const KeyCode t_key)
    {
        auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state = glfwGetKey(window, static_cast<int32_t>(t_key));

        return state == GLFW_PRESS;
    }

    bool Input::isMouseButtonPressed(const MouseCode t_button)
    {
        auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(t_button));
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::getMousePosition()
    {
        auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }

    float Input::getMouseX()
    {
        return getMousePosition().x;
    }

    float Input::getMouseY()
    {
        return getMousePosition().y;
    }
}
