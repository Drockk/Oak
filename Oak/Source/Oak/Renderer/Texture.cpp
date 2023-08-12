#include "oakpch.hpp"
#include "Oak/Renderer/Texture.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Platform/OpenGL/Texture.hpp"

namespace oak {
    Ref<Texture2D> Texture2D::create(const TextureSpecification& specification)
    {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::Texture2D>(specification);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::create(const std::string& path)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::Texture2D>(path);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
