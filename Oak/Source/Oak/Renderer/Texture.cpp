#include "oakpch.hpp"
#include "Oak/Renderer/Texture.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Platform/OpenGL/Texture.hpp"

namespace oak
{
    Ref<Texture2D> Texture2D::create(const TextureSpecification& t_specification)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<openGL::Texture2D>(t_specification);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::create(const std::string& t_path)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<openGL::Texture2D>(t_path);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
