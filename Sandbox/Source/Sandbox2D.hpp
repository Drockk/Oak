#pragma once

#include <Oak.hpp>

class Sandbox2D : public oak::Layer
{
public:
    Sandbox2D();
    ~Sandbox2D() override = default;

    void onAttach() override;
    void onDetach() override;

    void onUpdate(oak::Timestep t_timestep) override;
    void onImGuiRender() override;
    void onEvent(oak::Event& t_event) override;
private:
    oak::OrthographicCameraController m_CameraController;

    // Temp
    oak::Ref<oak::VertexArray> m_SquareVA;
    oak::Ref<oak::Shader> m_FlatColorShader;

    oak::Ref<oak::Texture2D> m_CheckerboardTexture;

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};
