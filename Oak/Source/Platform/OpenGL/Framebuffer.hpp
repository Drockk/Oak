#pragma once

#include "Oak/Renderer/Framebuffer.hpp"

namespace openGL
{
    class Framebuffer : public oak::Framebuffer
    {
    public:
        Framebuffer(const oak::FramebufferSpecification& spec);
        ~Framebuffer() override;

        void invalidate();

        void bind() override;
        void unbind() override;

        void resize(uint32_t width, uint32_t height) override;
        int readPixel(uint32_t attachmentIndex, int x, int y) override;

        void clearAttachment(uint32_t attachmentIndex, int value) override;

        uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override
        {
            OAK_CORE_ASSERT(index < m_ColorAttachments.size());
            return m_ColorAttachments[index];
        }

        const oak::FramebufferSpecification& getSpecification() const override
        {
            return m_Specification;
        }

    private:
        uint32_t m_RendererID = 0;
        oak::FramebufferSpecification m_Specification;

        std::vector<oak::FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
        oak::FramebufferTextureSpecification m_DepthAttachmentSpecification = oak::FramebufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };
}
