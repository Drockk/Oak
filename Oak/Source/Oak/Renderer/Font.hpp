#pragma once

#include <filesystem>

#include "Oak/Core/Base.hpp"
#include "Oak/Renderer/Texture.hpp"

namespace oak
{
    struct MSDFData;

    class Font
    {
    public:
        Font(const std::filesystem::path& t_font);
        ~Font();

        const MSDFData* getMSDFData() const { return m_Data; }
        Ref<Texture2D> getAtlasTexture() const { return m_AtlasTexture; }

        static Ref<Font> getDefault();
    private:
        MSDFData* m_Data{};
        Ref<Texture2D> m_AtlasTexture;
    };
}
