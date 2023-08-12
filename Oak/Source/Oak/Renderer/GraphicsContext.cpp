#include "oakpch.hpp"
#include "Oak/Renderer/GraphicsContext.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Platform/OpenGL/Context.hpp"

namespace oak {
    Scope<GraphicsContext> GraphicsContext::create(void* window)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createScope<opengl::Context>(static_cast<GLFWwindow*>(window));
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
