#pragma once

#include "Oak/Renderer/Texture.hpp"

#include <glad/gl.h>

namespace openGL
{
    class Texture2D : public oak::Texture2D
    {
    public:
        Texture2D(const oak::TextureSpecification& specification);
        Texture2D(const std::string& path);
        ~Texture2D() override;

        const oak::TextureSpecification& getSpecification() const override
        {
            return m_Specification;
        }

        uint32_t getWidth() const override
        {
            return m_Width;
        }

        uint32_t getHeight() const override
        {
            return m_Height;
        }

        uint32_t getRendererID() const override
        {
            return m_RendererID;
        }

        const std::string& getPath() const override
        {
            return m_Path;
        }

        void setData(void* data, uint32_t size) override;

        void bind(uint32_t slot = 0) const override;

        bool isLoaded() const override
        {
            return m_IsLoaded;
        }

        bool operator==(const Texture& other) const override
        {
            return m_RendererID == other.getRendererID();
        }
    private:
        oak::TextureSpecification m_Specification;

        std::string m_Path;
        bool m_IsLoaded = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };
}
