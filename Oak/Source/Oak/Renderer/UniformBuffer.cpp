#include "oakpch.hpp"
#include "Oak/Renderer/UniformBuffer.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Platform/OpenGL/UniformBuffer.hpp"

namespace oak
{
    Ref<UniformBuffer> UniformBuffer::create(uint32_t t_size, uint32_t t_binding)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateRef<openGL::UniformBuffer>(t_size, t_binding);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
