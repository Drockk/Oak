#pragma once

#include "Oak/Renderer/Buffer.hpp"

namespace opengl {
    class VertexBuffer final : public oak::VertexBuffer
    {
    public:
        VertexBuffer(uint32_t t_size);
        VertexBuffer(std::span<float> t_indicies);
        ~VertexBuffer() override;

        constexpr auto bind() -> void const override;
        constexpr auto unbind() -> void const override;

        constexpr auto setData(std::span<std::byte> t_indicies) -> void override;

        constexpr auto getLayout() const -> const oak::BufferLayout& override
        {
            return m_Layout;
        }

        constexpr auto setLayout(const oak::BufferLayout& t_layout) -> void override
        {
            m_Layout = t_layout;
        }

    private:
        uint32_t m_RendererID{};
        oak::BufferLayout m_Layout{};
    };

    class IndexBuffer final : public oak::IndexBuffer
    {
    public:
        IndexBuffer(std::span<uint32_t> t_indicies);
        ~IndexBuffer() override;

        constexpr auto bind() -> void const override;
        constexpr auto unbind() -> void const override;

        constexpr auto getCount() -> uint32_t const override
        {
            return m_Count;
        }

    private:
        uint32_t m_RendererID{};
        uint32_t m_Count{};
    };
}
