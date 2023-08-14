#include "oakpch.hpp"
#include "Platform/OpenGL/RendererAPI.hpp"

#include <glad/gl.h>

namespace opengl {
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

    void RendererAPI::setViewport(std::pair<uint32_t, uint32_t> t_position, std::pair<uint32_t, uint32_t> t_resolution)
    {
        auto [x, y] = t_position;
        auto [width, height] = t_resolution;
        glViewport(x, y, width, height);
    }

    void RendererAPI::setClearColor(const glm::vec4& t_color)
    {
        glClearColor(t_color.r, t_color.g, t_color.b, t_color.a);
    }

    void RendererAPI::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RendererAPI::drawIndexed(const oak::Ref<oak::VertexArray>& t_vertexArray, uint32_t t_indexCount)
    {
        t_vertexArray->bind();
        auto count = t_indexCount ? t_indexCount : t_vertexArray->getIndexBuffer()->getCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void RendererAPI::drawLines(const oak::Ref<oak::VertexArray>& t_vertexArray, uint32_t t_vertexCount)
    {
        t_vertexArray->bind();
        glDrawArrays(GL_LINES, 0, t_vertexCount);
    }

    void RendererAPI::setLineWidth(float t_width)
    {
        glLineWidth(t_width);
    }
}
