#include "oakpch.hpp"
#include "Oak/Renderer/Buffer.hpp"

#include "Oak/Renderer/Renderer.hpp"

#include "Platform/OpenGL/OpenGLBuffer.hpp"

namespace oak
{
    Ref<VertexBuffer> VertexBuffer::create(uint32_t t_size)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLVertexBuffer>(t_size);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::create(float* t_vertices, uint32_t t_size)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLVertexBuffer>(t_vertices, t_size);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::create(uint32_t* t_indices, uint32_t t_size)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLIndexBuffer>(t_indices, t_size);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

}
