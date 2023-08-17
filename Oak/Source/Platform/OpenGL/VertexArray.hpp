#pragma once

#include "Oak/Renderer/VertexArray.hpp"

namespace opengl {
    class VertexArray final : public oak::VertexArray
    {
    public:
        VertexArray();
        ~VertexArray() override;

        void bind() const override;
        void unbind() const override;

        void addVertexBuffer(const oak::Ref<oak::VertexBuffer>& vertexBuffer) override;
        void setIndexBuffer(const oak::Ref<oak::IndexBuffer>& indexBuffer) override;

        const std::vector<oak::Ref<oak::VertexBuffer>>& getVertexBuffers() const { return m_VertexBuffers; }
        const oak::Ref<oak::IndexBuffer>& getIndexBuffer() const { return m_IndexBuffer; }

    private:
        uint32_t m_RendererID{};
        uint32_t m_VertexBufferIndex{ 0 };
        std::vector<oak::Ref<oak::VertexBuffer>> m_VertexBuffers;
        oak::Ref<oak::IndexBuffer> m_IndexBuffer;
    };
}
