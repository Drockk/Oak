#pragma once
#include <Oak.hpp>
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include <Oak/Renderer/EditorCamera.hpp>

class EditorLayer final : public oak::Layer
{
public:
    EditorLayer();
    ~EditorLayer() override = default;

    void onAttach() override;
    void onDetach() override;

    void onUpdate(oak::Timestep ts) override;
    void onImGuiRender() override;
    void onEvent(oak::Event& e) override;

private:
    bool onKeyPressed(oak::KeyPressedEvent& e);
    bool onMouseButtonPressed(oak::MouseButtonPressedEvent& e);

    void onOverlayRender();

    void newProject();
    bool openProject();
    void openProject(const std::filesystem::path& path);
    void saveProject();

    void newScene();
    void openScene();
    void openScene(const std::filesystem::path& path);
    void saveScene();
    void saveSceneAs();

    void serializeScene(oak::Ref<oak::Scene> scene, const std::filesystem::path& path);

    void onScenePlay();
    void onSceneSimulate();
    void onSceneStop();
    void onScenePause();

    void onDuplicateEntity();

    // UI Panels
    void UI_Toolbar();

    oak::OrthographicCameraController m_CameraController;

    // Temp
    oak::Ref<oak::VertexArray> m_SquareVA;
    oak::Ref<oak::Shader> m_FlatColorShader;
    oak::Ref<oak::Framebuffer> m_Framebuffer;

    oak::Ref<oak::Scene> m_ActiveScene;
    oak::Ref<oak::Scene> m_EditorScene;
    std::filesystem::path m_EditorScenePath;
    oak::Entity m_SquareEntity;
    oak::Entity m_CameraEntity;
    oak::Entity m_SecondCamera;

    oak::Entity m_HoveredEntity;

    bool m_PrimaryCamera = true;

    oak::EditorCamera m_EditorCamera;

    oak::Ref<oak::Texture2D> m_CheckerboardTexture;

    bool m_ViewportFocused = false, m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    glm::vec2 m_ViewportBounds[2];

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

    int m_GizmoType = -1;

    bool m_ShowPhysicsColliders = false;

    enum class SceneState
    {
        Edit = 0, Play = 1, Simulate = 2
    };
    SceneState m_SceneState = SceneState::Edit;

    // Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    oak::Scope<ContentBrowserPanel> m_ContentBrowserPanel;

    // Editor resources
    oak::Ref<oak::Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;
};
