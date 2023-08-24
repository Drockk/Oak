#include "oakpch.hpp"
#include "Oak/Renderer/RendererAPI.hpp"

#include "Platform/OpenGL/RendererAPI.hpp"

namespace oak {
    Scope<RendererAPI> RendererAPI::create()
    {
        switch (s_API) {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createScope<opengl::RendererAPI>();
        case RendererAPI::API::Vulkan:
            return createScope<vulkan::RendererAPI>();
        }

        throw std::runtime_error("Unknown RendererAPI!");
        return nullptr;
    }
}
