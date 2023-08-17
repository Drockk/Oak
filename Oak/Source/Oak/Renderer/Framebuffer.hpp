#pragma once

#include "Oak/Core/Base.hpp"

namespace oak {
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
        FramebufferTextureSpecification(FramebufferTextureFormat format): textureFormat(format) {}

        FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments): attachments(attachments) {}

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

        virtual void resize(std::pair<uint32_t, uint32_t> t_pair) = 0;
        virtual int readPixel(uint32_t t_attachmentIndex, std::pair<int, int> t_pos) = 0;

        virtual void clearAttachment(uint32_t attachmentIndex, int value) = 0;

        virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const = 0;

        virtual const FramebufferSpecification& getSpecification() const = 0;

        static Ref<Framebuffer> create(const FramebufferSpecification& spec);
    };
}
