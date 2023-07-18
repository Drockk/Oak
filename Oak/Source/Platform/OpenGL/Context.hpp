#pragma once
#include "Oak/Renderer/GraphicsContext.hpp"

struct GLFWwindow;

namespace openGL
{
    class Context : public oak::GraphicsContext
    {
    public:
        Context(GLFWwindow* windowHandle);

        void init() override;
        void swapBuffers() override;
    private:
        GLFWwindow* m_WindowHandle;
    };
}
