#include "oakpch.hpp"
#include "Oak/Renderer/Buffer.hpp"

#include "Oak/Renderer/Renderer.hpp"

#include "Platform/OpenGL/Buffer.hpp"

namespace oak {
    Ref<VertexBuffer> VertexBuffer::create(uint32_t size)
    {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::VertexBuffer>(size);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size)
    {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::VertexBuffer>(vertices, size);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t size)
    {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::IndexBuffer>(indices, size);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
