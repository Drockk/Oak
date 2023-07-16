#include "oakpch.hpp"
#include "Oak/Renderer/Font.hpp"

#undef INFINITE
#include "msdf-atlas-gen.h"
#include "FontGeometry.h"
#include "GlyphGeometry.h"

#include "Oak/Renderer/MSDFData.hpp"

namespace oak
{
    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static Ref<Texture2D> createAndCacheAtlas(const std::string& t_fontName, float t_fontSize, const std::vector<msdf_atlas::GlyphGeometry>& t_glyphs,
        const msdf_atlas::FontGeometry& t_fontGeometry, uint32_t t_width, uint32_t t_height)
    {
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;

        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(t_width, t_height);
        generator.setAttributes(attributes);
        generator.setThreadCount(8);
        generator.generate(t_glyphs.data(), (int)t_glyphs.size());

        msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

        TextureSpecification spec;
        spec.Width = bitmap.width;
        spec.Height = bitmap.height;
        spec.Format = ImageFormat::RGB8;
        spec.GenerateMips = false;

        Ref<Texture2D> texture = Texture2D::create(spec);
        texture->setData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
        return texture;
    }

    Font::Font(const std::filesystem::path& t_filepath)
        : m_Data(new MSDFData())
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        OAK_CORE_ASSERT(ft);

        std::string fileString = t_filepath.string();

        // TODO(Yan): msdfgen::loadFontData loads from memory buffer which we'll need
        msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
        if (!font)
        {
            OAK_LOG_CORE_ERROR("Failed to load font: {}", fileString);
            return;
        }

        struct CharsetRange
        {
            uint32_t Begin, End;
        };

        // From imgui_draw.cpp
        static const CharsetRange charsetRanges[] =
        {
            { 0x0020, 0x00FF }
        };

        msdf_atlas::Charset charset;
        for (CharsetRange range : charsetRanges)
        {
            for (uint32_t c = range.Begin; c <= range.End; c++)
                charset.add(c);
        }

        double fontScale = 1.0;
        m_Data->fontGeometry = msdf_atlas::FontGeometry(&m_Data->glyphs);
        int glyphsLoaded = m_Data->fontGeometry.loadCharset(font, fontScale, charset);
        OAK_LOG_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());


        double emSize = 40.0;

        msdf_atlas::TightAtlasPacker atlasPacker;
        // atlasPacker.setDimensionsConstraint()
        atlasPacker.setPixelRange(2.0);
        atlasPacker.setMiterLimit(1.0);
        atlasPacker.setPadding(0);
        atlasPacker.setScale(emSize);
        int remaining = atlasPacker.pack(m_Data->glyphs.data(), (int)m_Data->glyphs.size());
        OAK_CORE_ASSERT(remaining == 0);

        int width, height;
        atlasPacker.getDimensions(width, height);
        emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
        // if MSDF || MTSDF

        uint64_t coloringSeed = 0;
        bool expensiveColoring = false;
        if (expensiveColoring)
        {
            msdf_atlas::Workload([&glyphs = m_Data->glyphs, &coloringSeed](int i, int threadNo) -> bool {
                unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
                glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
                return true;
                }, m_Data->glyphs.size()).finish(THREAD_COUNT);
        }
        else {
            unsigned long long glyphSeed = coloringSeed;
            for (msdf_atlas::GlyphGeometry& glyph : m_Data->glyphs)
            {
                glyphSeed *= LCG_MULTIPLIER;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
            }
        }


        m_AtlasTexture = createAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->glyphs, m_Data->fontGeometry, width, height);

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);
    }

    Font::~Font()
    {
        delete m_Data;
    }


    Ref<Font> Font::getDefault()
    {
        static Ref<Font> DefaultFont;
        if (!DefaultFont)
            DefaultFont = createRef<Font>("assets/fonts/opensans/OpenSans-Regular.ttf");

        return DefaultFont;
    }

}
