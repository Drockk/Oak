#include "oakpch.hpp"
#include "Oak/Renderer/Renderer.hpp"
#include "Oak/Renderer/Renderer2D.hpp"

namespace oak
{
    Scope<Renderer::SceneData> Renderer::s_SceneData = createScope<Renderer::SceneData>();

    void Renderer::init()
    {
        OAK_PROFILE_FUNCTION();

        RenderCommand::init();
        Renderer2D::init();
    }

    void Renderer::shutdown()
    {
        Renderer2D::shutdown();
    }

    void Renderer::onWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::setViewport(0, 0, width, height);
    }

    void Renderer::beginScene(OrthographicCamera& camera)
    {
        s_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
    }

    void Renderer::endScene()
    {
    }

    void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->bind();
        shader->setMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        shader->setMat4("u_Transform", transform);

        vertexArray->bind();
        RenderCommand::drawIndexed(vertexArray);
    }
}
