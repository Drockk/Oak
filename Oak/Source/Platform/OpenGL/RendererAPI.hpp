#pragma once

#include "Oak/Renderer/RendererAPI.hpp"

namespace opengl {
    class RendererAPI final : public oak::RendererAPI
    {
    public:
        void init() override;
        void setViewport(std::pair<uint32_t, uint32_t> t_position, std::pair<uint32_t, uint32_t> t_resolution) override;

        void setClearColor(const glm::vec4& t_color) override;
        void clear() override;

        void drawIndexed(const oak::Ref<oak::VertexArray>& t_vertexArray, uint32_t t_indexCount = 0) override;
        void drawLines(const oak::Ref<oak::VertexArray>& t_vertexArray, uint32_t t_vertexCount) override;

        void setLineWidth(float width) override;
    };
}
