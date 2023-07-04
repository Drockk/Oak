#pragma once

#include <Oak.hpp>

class ExampleLayer : public oak::Layer
{
public:
    ExampleLayer();
    ~ExampleLayer() override = default;

    void onAttach() override;
    void onDetach() override;

    void onUpdate(oak::Timestep t_timestep) override;
    void onImGuiRender() override;
    void onEvent(oak::Event& t_event) override;

private:
    oak::ShaderLibrary m_ShaderLibrary;
    oak::Ref<oak::Shader> m_Shader;
    oak::Ref<oak::VertexArray> m_VertexArray;

    oak::Ref<oak::Shader> m_FlatColorShader;
    oak::Ref<oak::VertexArray> m_SquareVA;

    oak::Ref<oak::Texture2D> m_Texture;
    oak::Ref<oak::Texture2D> m_ChernoLogoTexture;

    oak::OrthographicCameraController m_CameraController;
    glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};
