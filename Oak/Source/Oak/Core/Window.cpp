#include "oakpch.hpp"
#include "Oak/Core/Window.hpp"
#include "Platform/Windows/Window.hpp"

namespace oak
{
    std::unique_ptr<Window> Window::create(WindowData t_data)
    {
        return std::make_unique<Windows::Window>(t_data);
    }
}
