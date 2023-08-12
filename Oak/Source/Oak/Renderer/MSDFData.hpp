#pragma once

#include <vector>

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace oak {
    struct MSDFData
    {
        std::vector<msdf_atlas::GlyphGeometry> glyphs;
        msdf_atlas::FontGeometry fontGeometry;
    };
}
