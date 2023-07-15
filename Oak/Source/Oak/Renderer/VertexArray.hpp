#pragma once
#include "Oak/Renderer/Buffer.hpp"

#include <memory>

namespace oak
{
    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual void addVertexBuffer(const Ref<VertexBuffer>& t_vertexBuffer) = 0;
        virtual void setIndexBuffer(const Ref<IndexBuffer>& t_indexBuffer) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& getIndexBuffer() const = 0;

        static Ref<VertexArray> create();
    };
}