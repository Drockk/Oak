#pragma once

#include "Oak/Renderer/VertexArray.hpp"

#include <glm/glm.hpp>

namespace oak {
    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0,
            OpenGL = 1,
            Vulkan = 2
        };

        virtual ~RendererAPI() = default;

        virtual void init() = 0;
        virtual void setViewport(std::pair<uint32_t, uint32_t> t_position, std::pair<uint32_t, uint32_t> t_resolution) = 0;
        virtual void setClearColor(const glm::vec4& color) = 0;
        virtual void clear() = 0;

        virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
        virtual void drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
        
        virtual void setLineWidth(float width) = 0;

        static API getAPI() { return s_API; }
        static Scope<RendererAPI> create();

    private:
        inline static API s_API{ RendererAPI::API::Vulkan };
    };
}
