#include "oakpch.hpp"
#include "Platform/OpenGL/Buffer.hpp"

#include <glad/gl.h>

namespace opengl {

    /////////////////////////////////////////////////////////////////////////////
    // VertexBuffer /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    VertexBuffer::VertexBuffer(uint32_t t_size)
    {
        OAK_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, t_size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer::VertexBuffer(std::span<float> t_indicies)
    {
        OAK_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, t_indicies.size(), t_indicies.data(), GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        OAK_PROFILE_FUNCTION();

        glDeleteBuffers(1, &m_RendererID);
    }

    constexpr auto VertexBuffer::bind() -> void const
    {
        OAK_PROFILE_FUNCTION();

        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    constexpr auto VertexBuffer::unbind() -> void const
    {
        OAK_PROFILE_FUNCTION();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    constexpr auto VertexBuffer::setData(std::span<std::byte> t_indicies) -> void
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, t_indicies.size(), t_indicies.data());
    }

    /////////////////////////////////////////////////////////////////////////////
    // IndexBuffer //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    IndexBuffer::IndexBuffer(std::span<uint32_t> t_indicies) : m_Count(t_indicies.size())
    {
        OAK_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID);

        // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
        // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, t_indicies.size_bytes(), t_indicies.data(), GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        OAK_PROFILE_FUNCTION();

        glDeleteBuffers(1, &m_RendererID);
    }

    constexpr auto IndexBuffer::bind() -> void const
    {
        OAK_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    constexpr auto IndexBuffer::unbind() -> void const
    {
        OAK_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
