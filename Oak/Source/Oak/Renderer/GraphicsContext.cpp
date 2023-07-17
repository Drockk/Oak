#include "oakpch.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Oak/Renderer/GraphicsContext.hpp"
#include "Platform/OpenGL/Context.hpp"

namespace oak
{
    Scope<GraphicsContext> GraphicsContext::create(std::any t_window)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createScope<openGL::Context>(std::any_cast<GLFWwindow*>(t_window));
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
