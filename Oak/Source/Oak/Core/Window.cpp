#include "oakpch.hpp"
#include "Oak/Core/Window.hpp"

#ifdef OAK_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsWindow.hpp"
#endif

namespace oak
{
    Scope<Window> Window::create(const WindowProps& t_props)
    {
    #ifdef OAK_PLATFORM_WINDOWS
        return createScope<WindowsWindow>(t_props);
    #else
        OAK_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
    #endif
    }
}
