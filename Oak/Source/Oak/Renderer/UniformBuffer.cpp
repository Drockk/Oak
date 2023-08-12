#include "oakpch.hpp"
#include "UniformBuffer.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Platform/OpenGL/UniformBuffer.hpp"

namespace oak {
    Ref<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::UniformBuffer>(size, binding);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
