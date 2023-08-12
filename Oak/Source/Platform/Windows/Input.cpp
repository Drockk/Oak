#include "oakpch.hpp"
#include "Oak/Core/Input.hpp"

#include "Oak/Core/Application.hpp"
#include <GLFW/glfw3.h>

bool oak::Input::isKeyPressed(const oak::KeyCode key)
{
    auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    auto state = glfwGetKey(window, static_cast<int32_t>(key));
    return state == GLFW_PRESS;
}

bool oak::Input::isMouseButtonPressed(const oak::MouseCode button)
{
    auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
    return state == GLFW_PRESS;
}

glm::vec2 oak::Input::getMousePosition()
{
    auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return { static_cast<float>(xpos), static_cast<float>(ypos) };
}

float oak::Input::getMouseX()
{
    return getMousePosition().x;
}

float oak::Input::getMouseY()
{
    return getMousePosition().y;
}
