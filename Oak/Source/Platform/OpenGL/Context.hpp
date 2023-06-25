#pragma once
#include "Oak/Renderer/GraphicsContext.hpp"

#include <any>

struct GLFWwindow;

namespace openGL
{
    class Context: public oak::GraphicsContext
    {
    public:
        Context(std::any t_windowHandle);

        void init() override;
        void swapBuffers() override;

    private:
        GLFWwindow* m_WindowHandle;
    };
}
