#pragma once
#include "Oak/Renderer/VertexArray.hpp"

#include <glm/glm.hpp>

namespace oak
{
    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0,
            OpenGL = 1,
        };

        virtual ~RendererAPI() = default;

        virtual void init() = 0;
        virtual void setViewport(uint32_t t_x, uint32_t t_y, uint32_t t_width, uint32_t t_height) = 0;
        virtual void setClearColor(const glm::vec4& t_color) = 0;
        virtual void clear() = 0;

        virtual void drawIndexed(const Ref<VertexArray>& t_vertexArray, uint32_t t_indexCount = 0) = 0;
        virtual void drawLines(const Ref<VertexArray>& t_vertexArray, uint32_t t_vertexCount) = 0;

        virtual void setLineWidth(float width) = 0;

        static API getAPI()
        {
            return s_API;
        }
        static Scope<RendererAPI> create();

    private:
        inline static API s_API{ API::OpenGL };
    };
}
