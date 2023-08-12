#include "oakpch.hpp"
#include "Oak/Renderer/Framebuffer.hpp"

#include "Oak/Renderer/Renderer.hpp"

#include "Platform/OpenGL/Framebuffer.hpp"

namespace oak {
    Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec)
    {
        switch (Renderer::getAPI()) {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<opengl::Framebuffer>(spec);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
