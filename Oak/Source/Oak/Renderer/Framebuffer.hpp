#pragma once

#include "Oak/Core/Base.hpp"

namespace oak
{
    enum class FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat t_format): textureFormat(t_format) {}

        FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> t_attachments): attachments(t_attachments) {}

        std::vector<FramebufferTextureSpecification> attachments;
    };

    struct FramebufferSpecification
    {
        uint32_t width = 0, height = 0;
        FramebufferAttachmentSpecification attachments;
        uint32_t samples = 1;

        bool swapChainTarget = false;
    };

    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void bind() = 0;
        virtual void unbind() = 0;

        virtual void resize(uint32_t t_width, uint32_t t_height) = 0;
        virtual int readPixel(uint32_t t_attachmentIndex, int t_x, int t_y) = 0;

        virtual void clearAttachment(uint32_t t_attachmentIndex, int t_value) = 0;

        virtual uint32_t getColorAttachmentRendererID(uint32_t t_index = 0) const = 0;

        virtual const FramebufferSpecification& getSpecification() const = 0;

        static Ref<Framebuffer> create(const FramebufferSpecification& t_spec);
    };
}
