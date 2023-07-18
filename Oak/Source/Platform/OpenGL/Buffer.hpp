#pragma once
#include "Oak/Renderer/Buffer.hpp"

namespace openGL
{
    class VertexBuffer : public oak::VertexBuffer
    {
    public:
        VertexBuffer(uint32_t size);
        VertexBuffer(float* vertices, uint32_t size);
        ~VertexBuffer() override;

        void bind() const override;
        void unbind() const override;

        void setData(const void* data, uint32_t size) override;

        const oak::BufferLayout& getLayout() const override
        {
            return m_Layout;
        }

        void setLayout(const oak::BufferLayout& layout) override
        {
            m_Layout = layout;
        }

    private:
        uint32_t m_RendererID;
        oak::BufferLayout m_Layout;
    };

    class IndexBuffer : public oak::IndexBuffer
    {
    public:
        IndexBuffer(uint32_t* indices, uint32_t count);
        ~IndexBuffer() override;

        void bind() const;
        void unbind() const;

        uint32_t getCount() const
        {
            return m_Count;
        }

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}
