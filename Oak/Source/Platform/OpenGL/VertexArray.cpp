#include "oakpch.hpp"
#include "Platform/OpenGL/VertexArray.hpp"

#include <glad/gl.h>

namespace opengl {
    static GLenum shaderDataTypeToOpenGLBaseType(oak::ShaderDataType type)
    {
        switch (type)
        {
            case oak::ShaderDataType::Float:
                return GL_FLOAT;
            case oak::ShaderDataType::Float2:
                return GL_FLOAT;
            case oak::ShaderDataType::Float3:
                return GL_FLOAT;
            case oak::ShaderDataType::Float4:
                return GL_FLOAT;
            case oak::ShaderDataType::Mat3:
                return GL_FLOAT;
            case oak::ShaderDataType::Mat4:
                return GL_FLOAT;
            case oak::ShaderDataType::Int:
                return GL_INT;
            case oak::ShaderDataType::Int2:
                return GL_INT;
            case oak::ShaderDataType::Int3:
                return GL_INT;
            case oak::ShaderDataType::Int4:
                return GL_INT;
            case oak::ShaderDataType::Bool:
                return GL_BOOL;
        }

        OAK_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    VertexArray::VertexArray()
    {
        OAK_PROFILE_FUNCTION();

        glCreateVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        OAK_PROFILE_FUNCTION();

        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::bind() const
    {
        OAK_PROFILE_FUNCTION();

        glBindVertexArray(m_RendererID);
    }

    void VertexArray::unbind() const
    {
        OAK_PROFILE_FUNCTION();

        glBindVertexArray(0);
    }

    void VertexArray::addVertexBuffer(const oak::Ref<oak::VertexBuffer>& vertexBuffer)
    {
        OAK_PROFILE_FUNCTION();

        OAK_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

        glBindVertexArray(m_RendererID);
        vertexBuffer->bind();

        const auto& layout = vertexBuffer->getLayout();
        for (const auto& element : layout) {
            switch (element.type) {
                case oak::ShaderDataType::Float:
                case oak::ShaderDataType::Float2:
                case oak::ShaderDataType::Float3:
                case oak::ShaderDataType::Float4:
                {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(m_VertexBufferIndex,
                        element.getComponentCount(),
                        shaderDataTypeToOpenGLBaseType(element.type),
                        element.normalized ? GL_TRUE : GL_FALSE,
                        layout.getStride(),
                        (const void*)element.offset);
                    m_VertexBufferIndex++;
                    break;
                }
                case oak::ShaderDataType::Int:
                case oak::ShaderDataType::Int2:
                case oak::ShaderDataType::Int3:
                case oak::ShaderDataType::Int4:
                case oak::ShaderDataType::Bool:
                {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribIPointer(m_VertexBufferIndex,
                        element.getComponentCount(),
                        shaderDataTypeToOpenGLBaseType(element.type),
                        layout.getStride(),
                        (const void*)element.offset);
                    m_VertexBufferIndex++;
                    break;
                }
                case oak::ShaderDataType::Mat3:
                case oak::ShaderDataType::Mat4:
                {
                    uint8_t count = element.getComponentCount();
                    for (uint8_t i = 0; i < count; i++)
                    {
                        glEnableVertexAttribArray(m_VertexBufferIndex);
                        glVertexAttribPointer(m_VertexBufferIndex,
                            count,
                            shaderDataTypeToOpenGLBaseType(element.type),
                            element.normalized ? GL_TRUE : GL_FALSE,
                            layout.getStride(),
                            (const void*)(element.offset + sizeof(float) * count * i));
                        glVertexAttribDivisor(m_VertexBufferIndex, 1);
                        m_VertexBufferIndex++;
                    }
                    break;
                }
                default:
                    OAK_CORE_ASSERT(false, "Unknown ShaderDataType!");
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::setIndexBuffer(const oak::Ref<oak::IndexBuffer>& indexBuffer)
    {
        OAK_PROFILE_FUNCTION();

        glBindVertexArray(m_RendererID);
        indexBuffer->bind();

        m_IndexBuffer = indexBuffer;
    }
}
