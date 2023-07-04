#include "oakpch.hpp"

#include "Oak/Renderer/GraphicsContext.hpp"
#include "Platform/OpenGL/Context.hpp"

namespace oak
{
    std::shared_ptr<GraphicsContext> GraphicsContext::create(std::any t_window)
    {
        return std::make_shared<openGL::Context>(t_window);
    }
}
