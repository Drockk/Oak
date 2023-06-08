#include "oakpch.hpp"
#include "Oak/Core/Window.hpp"
#include "Platform/Windows/Window.hpp"

namespace oak
{
    std::unique_ptr<Window> Window::create(const std::string& name, std::pair<uint32_t, uint32_t> resolution)
    {
        return std::make_unique<Windows::Window>(name, resolution);
    }
}
