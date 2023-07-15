#include "oakpch.hpp"
#include "Oak/Renderer/VertexArray.hpp"
#include "Oak/Renderer/Renderer.hpp"

#include "Platform/OpenGL/VertexArray.hpp"

namespace oak
{
    Ref<VertexArray> VertexArray::create()
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<openGL::VertexArray>();
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
