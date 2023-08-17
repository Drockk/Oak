#pragma once

#include "Oak/Renderer/Framebuffer.hpp"

namespace opengl {
    class Framebuffer final : public oak::Framebuffer
    {
    public:
        Framebuffer(const oak::FramebufferSpecification& t_spec);
        ~Framebuffer() override;

        constexpr auto invalidate() -> void;

        constexpr auto bind() -> void override;
        constexpr auto unbind() -> void override;

        constexpr auto resize(std::pair<uint32_t, uint32_t> t_pair) -> void override;
        constexpr auto readPixel(uint32_t t_attachmentIndex, std::pair<int, int> t_pos) -> int override;

        constexpr auto clearAttachment(uint32_t t_attachmentIndex, int t_value) -> void override;

        constexpr auto getColorAttachmentRendererID(uint32_t t_index = 0) const -> uint32_t override
        {
            if (t_index >= m_ColorAttachments.size()) {
                throw std::invalid_argument("attachment index is to big");
            }

            return m_ColorAttachments[t_index];
        }

        auto getSpecification() const -> const oak::FramebufferSpecification& override
        {
            return m_Specification;
        }

    private:
        uint32_t m_RendererID{ 0 };
        oak::FramebufferSpecification m_Specification;

        std::vector<oak::FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
        oak::FramebufferTextureSpecification m_DepthAttachmentSpecification{ oak::FramebufferTextureFormat::None };

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment{ 0 };
    };
}
