#pragma once
#include <glad/gl.h>

#include <string>

namespace oak
{
    class Texture
    {
    public:
        Texture() = default;
        ~Texture() = default;

        void loadFromFile(const std::string& filename);
        void bindTexture() const;
        GLuint getID() const;

    private:
        GLuint m_TextureID{ 0 };
    };
}
