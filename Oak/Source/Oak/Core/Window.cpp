#include "oakpch.hpp"
#include "Oak/Core/Window.hpp"

#ifdef OAK_PLATFORM_WINDOWS
    #include "Platform/Windows/Window.hpp"
#endif

namespace oak {
    Scope<Window> Window::create(const WindowProps& props)
    {
    #ifdef OAK_PLATFORM_WINDOWS
        return createScope<windows::Window>(props);
    #else
        OAK_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
    #endif
    }
}
