#include "oakpch.hpp"
#include "Oak/Renderer/RenderCommand.hpp"

namespace oak {
    Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::create();
}
