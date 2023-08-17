#include "oakpch.hpp"
#include "Platform/OpenGL/Framebuffer.hpp"

#include <glad/gl.h>

namespace opengl {
    static const auto s_MaxFramebufferSize{ 8192 };

    namespace utils {
        constexpr static auto textureTarget(bool t_multisampled) -> GLenum
        {
            return t_multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        constexpr static auto createTextures(bool t_multisampled, std::span<uint32_t> t_textures) -> void
        {
            glCreateTextures(textureTarget(t_multisampled), t_textures.size(), t_textures.data());
        }

        constexpr static auto bindTexture(bool t_multisampled, uint32_t t_id) -> void
        {
            glBindTexture(textureTarget(t_multisampled), t_id);
        }

        constexpr static auto attachColorTexture(uint32_t t_id, int t_samples, GLenum t_internalFormat, GLenum t_format, std::pair<uint32_t, uint32_t> t_resolution, int t_index) -> void
        {
            const auto multisampled = t_samples > 1;
            const auto [width, height] = t_resolution;
            if (multisampled) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, t_samples, t_internalFormat, width, height, GL_FALSE);
            }
            else {
                glTexImage2D(GL_TEXTURE_2D, 0, t_internalFormat, width, height, 0, t_format, GL_UNSIGNED_BYTE, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + t_index, textureTarget(multisampled), t_id, 0);
        }

        constexpr static auto attachDepthTexture(uint32_t t_id, int t_samples, GLenum t_format, GLenum t_attachmentType, std::pair<uint32_t, uint32_t> t_resolution) -> void
        {
            const auto multisampled = t_samples > 1;
            const auto [width, height] = t_resolution;
            if (multisampled) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, t_samples, t_format, width, height, GL_FALSE);
            }
            else {
                glTexStorage2D(GL_TEXTURE_2D, 1, t_format, width, height);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, t_attachmentType, textureTarget(multisampled), t_id, 0);
        }

        constexpr static auto isDepthFormat(oak::FramebufferTextureFormat t_format) -> bool
        {
            if (t_format == oak::FramebufferTextureFormat::DEPTH24STENCIL8) {
                return true;
            }

            return false;
        }

        constexpr auto oakFBTextureFormatToGL(oak::FramebufferTextureFormat t_format) -> GLenum
        {
            switch (t_format) {
            case oak::FramebufferTextureFormat::RGBA8:
                return GL_RGBA8;
            case oak::FramebufferTextureFormat::RED_INTEGER:
                return GL_RED_INTEGER;
            }

            throw std::invalid_argument("Invalid format");
            return 0;
        }
    }

    Framebuffer::Framebuffer(const oak::FramebufferSpecification& t_spec): m_Specification(t_spec)
    {
        for (auto spec : m_Specification.attachments.attachments) {
            if (!utils::isDepthFormat(spec.textureFormat)) {
                m_ColorAttachmentSpecifications.emplace_back(spec);
            }
            else {
                m_DepthAttachmentSpecification = spec;
            }
        }

        invalidate();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    constexpr auto Framebuffer::invalidate() -> void
    {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);

            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
        }

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        const auto multisample = m_Specification.samples > 1;

        // Attachments
        if (m_ColorAttachmentSpecifications.size()) {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
            utils::createTextures(multisample, m_ColorAttachments);

            for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
                utils::bindTexture(multisample, m_ColorAttachments[i]);
                switch (m_ColorAttachmentSpecifications[i].textureFormat) {
                case oak::FramebufferTextureFormat::RGBA8:
                    utils::attachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_RGBA8, GL_RGBA, { m_Specification.width, m_Specification.height }, i);
                    break;
                case oak::FramebufferTextureFormat::RED_INTEGER:
                    utils::attachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_R32I, GL_RED_INTEGER, { m_Specification.width, m_Specification.height }, i);
                    break;
                }
            }
        }

        if (m_DepthAttachmentSpecification.textureFormat != oak::FramebufferTextureFormat::None) {
            utils::createTextures(multisample, { &m_DepthAttachment, 1 });
            utils::bindTexture(multisample, m_DepthAttachment);
            switch (m_DepthAttachmentSpecification.textureFormat) {
            case oak::FramebufferTextureFormat::DEPTH24STENCIL8:
                utils::attachDepthTexture(m_DepthAttachment, m_Specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, { m_Specification.width, m_Specification.height });
                break;
            }
        }

        if (m_ColorAttachments.size() > 1) {
            if (m_ColorAttachments.size() > 4) {
                throw std::runtime_error("Color attachment number is bigger than 4");
            }
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.empty()) {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Framebuffer is incomplete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    constexpr auto Framebuffer::bind() -> void
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.width, m_Specification.height);
    }

    constexpr auto Framebuffer::unbind() -> void
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    constexpr auto Framebuffer::resize(std::pair<uint32_t, uint32_t> t_pair) -> void
    {
        const auto [width, height] = t_pair;
        if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
            OAK_LOG_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
            return;
        }
        m_Specification.width = width;
        m_Specification.height = height;

        invalidate();
    }

    constexpr auto Framebuffer::readPixel(uint32_t t_attachmentIndex, std::pair<int, int> t_pos) -> int
    {
        if (t_attachmentIndex > m_ColorAttachments.size()) {
            throw std::runtime_error("attachment index is too big");
        }

        glReadBuffer(GL_COLOR_ATTACHMENT0 + t_attachmentIndex);
        int pixelData;
        const auto [x, y] = t_pos;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

        return pixelData;
    }

    constexpr auto Framebuffer::clearAttachment(uint32_t t_attachmentIndex, int t_value) -> void
    {
        if (t_attachmentIndex > m_ColorAttachments.size()) {
            throw std::runtime_error("attachment index is too big");
        }

        auto& spec = m_ColorAttachmentSpecifications[t_attachmentIndex];
        glClearTexImage(m_ColorAttachments[t_attachmentIndex], 0, utils::oakFBTextureFormatToGL(spec.textureFormat), GL_INT, &t_value);
    }
}
