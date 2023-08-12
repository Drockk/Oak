#pragma once

#include "Oak/Renderer/RendererAPI.hpp"

namespace opengl {
    class RendererAPI final : public oak::RendererAPI
    {
    public:
        void init() override;
        void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        void setClearColor(const glm::vec4& color) override;
        void clear() override;

        void drawIndexed(const oak::Ref<oak::VertexArray>& vertexArray, uint32_t indexCount = 0) override;
        void drawLines(const oak::Ref<oak::VertexArray>& vertexArray, uint32_t vertexCount) override;

        void setLineWidth(float width) override;
    };
}
