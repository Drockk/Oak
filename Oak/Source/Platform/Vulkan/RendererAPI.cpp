#include "oakpch.hpp"
#include "Platform/Vulkan/RendererAPI.hpp"

namespace vulkan {
    void RendererAPI::init()
    {
        throw std::runtime_error("Not Implemented!");
    }

    void RendererAPI::setViewport(std::pair<uint32_t, uint32_t> t_position, std::pair<uint32_t, uint32_t> t_resolution)
    {
        throw std::runtime_error("Not Implemented!");
    }

    void RendererAPI::setClearColor(const glm::vec4& t_color)
    {
        throw std::runtime_error("Not Implemented!");
    }

    void RendererAPI::clear()
    {
        throw std::runtime_error("Not Implemented!");
    }

    void RendererAPI::drawIndexed(const oak::Ref<oak::VertexArray>& t_vertexArray, uint32_t t_indexCount)
    {
        throw std::runtime_error("Not Implemented!");
    }

    void RendererAPI::drawLines(const oak::Ref<oak::VertexArray>& t_vertexArray, uint32_t t_vertexCount)
    {
        throw std::runtime_error("Not Implemented!");
    }

    void RendererAPI::setLineWidth(float width)
    {
        throw std::runtime_error("Not Implemented!");
    }
}
