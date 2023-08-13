#pragma once

#include "Oak/Renderer/GraphicsContext.hpp"

struct GLFWwindow;

namespace opengl {
    class Context : public oak::GraphicsContext
    {
    public:
        Context(GLFWwindow* windowHandle);

        auto init() -> void override;
        auto swapBuffers() -> void override;

    private:
        GLFWwindow* m_WindowHandle{ nullptr };
    };
}
