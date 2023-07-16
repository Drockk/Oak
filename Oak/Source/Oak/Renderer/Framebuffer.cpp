#include "oakpch.hpp"
#include "Oak/Renderer/Framebuffer.hpp"
#include "Oak/Renderer/Renderer.hpp"

#include "Platform/OpenGL/OpenGLFramebuffer.hpp"

namespace oak
{
    Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& t_spec)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLFramebuffer>(t_spec);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
