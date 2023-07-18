#include "oakpch.hpp"
#include "Platform/OpenGL/RendererAPI.hpp"

#include <glad/gl.h>

namespace openGL
{
    void openGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            OAK_LOG_CORE_CRITICAL(message);
            return;
        case GL_DEBUG_SEVERITY_MEDIUM:
            OAK_LOG_CORE_ERROR(message);
            return;
        case GL_DEBUG_SEVERITY_LOW:
            OAK_LOG_CORE_WARN(message);
            return;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            OAK_LOG_CORE_TRACE(message);
            return;
        }

        OAK_CORE_ASSERT(false, "Unknown severity level!");
    }

    void RendererAPI::init()
    {
        OAK_PROFILE_FUNCTION();

#ifdef OAK_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
    }

    void RendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void RendererAPI::setClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RendererAPI::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RendererAPI::drawIndexed(const oak::Ref<oak::VertexArray>& vertexArray, uint32_t indexCount)
    {
        vertexArray->bind();
        uint32_t count = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void RendererAPI::drawLines(const oak::Ref<oak::VertexArray>& vertexArray, uint32_t vertexCount)
    {
        vertexArray->bind();
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

    void RendererAPI::setLineWidth(float width)
    {
        glLineWidth(width);
    }
}
