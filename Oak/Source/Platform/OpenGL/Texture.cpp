#include "oakpch.hpp"
#include "Platform/OpenGL/Texture.hpp"

#include <stb_image.h>

namespace opengl {
    namespace utils {
        static GLenum oakImageFormatToGLDataFormat(oak::ImageFormat format)
        {
            switch (format) {
            case oak::ImageFormat::RGB8:
                return GL_RGB;
            case oak::ImageFormat::RGBA8:
                return GL_RGBA;
            }

            OAK_CORE_ASSERT(false);
            return 0;
        }

        static GLenum oakImageFormatToGLInternalFormat(oak::ImageFormat format)
        {
            switch (format) {
            case oak::ImageFormat::RGB8:
                return GL_RGB8;
            case oak::ImageFormat::RGBA8:
                return GL_RGBA8;
            }

            OAK_CORE_ASSERT(false);
            return 0;
        }
    }

    Texture2D::Texture2D(const oak::TextureSpecification& specification) : m_Specification{ specification }, m_Resolution{ specification.width, specification.height }
    {
        OAK_PROFILE_FUNCTION();

        m_InternalFormat = utils::oakImageFormatToGLInternalFormat(specification.format);
        m_DataFormat = utils::oakImageFormatToGLDataFormat(specification.format);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, specification.width, specification.height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    Texture2D::Texture2D(const std::string& path): m_Path(path)
    {
        OAK_PROFILE_FUNCTION();

        int width{}, height{}, channels{};
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data{ nullptr };

        {
            OAK_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }

        if (data) {
            m_IsLoaded = true;
            m_Resolution = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4) {
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
            }
            else if (channels == 3) {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
            }

            m_InternalFormat = internalFormat;
            m_DataFormat = dataFormat;

            OAK_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

            glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
            glTextureStorage2D(m_RendererID, 1, internalFormat, width, height);

            glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
    }

    Texture2D::~Texture2D()
    {
        OAK_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void Texture2D::setData(void* data, uint32_t size)
    {
        OAK_PROFILE_FUNCTION();

        auto bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        auto [width, height] = m_Resolution;
        if (width * height * bpp != size) {
            throw std::invalid_argument("Data must be entire texture!");
        }

        glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    void Texture2D::bind(uint32_t slot) const
    {
        OAK_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);
    }
}
