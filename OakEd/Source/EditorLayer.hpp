#pragma once

#include <Oak.hpp>
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include "Oak/Renderer/EditorCamera.hpp"

class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer() override = default;

    void onAttach() override;
    void onDetach() override;

    void onUpdate(Timestep t_ts) override;
    void onImGuiRender() override;
    void onEvent(Event& t_e) override;
private:
    bool onKeyPressed(KeyPressedEvent& t_e);
    bool onMouseButtonPressed(MouseButtonPressedEvent& t_e);

    void onOverlayRender();

    void newProject();
    bool openProject();
    void openProject(const std::filesystem::path& t_path);
    void saveProject();

    void newScene();
    void openScene();
    void openScene(const std::filesystem::path& t_path);
    void saveScene();
    void saveSceneAs();

    void serializeScene(Ref<Scene> t_scene, const std::filesystem::path& t_path);

    void onScenePlay();
    void onSceneSimulate();
    void onSceneStop();
    void onScenePause();

    void onDuplicateEntity();

    // UI Panels
    void uiToolbar();
private:
    Oak::OrthographicCameraController m_CameraController;

    // Temp
    Ref<VertexArray> m_SquareVA;
    Ref<Shader> m_FlatColorShader;
    Ref<Framebuffer> m_Framebuffer;

    Ref<Scene> m_ActiveScene;
    Ref<Scene> m_EditorScene;
    std::filesystem::path m_EditorScenePath;
    Entity m_SquareEntity;
    Entity m_CameraEntity;
    Entity m_SecondCamera;

    Entity m_HoveredEntity;

    bool m_PrimaryCamera = true;

    EditorCamera m_EditorCamera;

    Ref<Texture2D> m_CheckerboardTexture;

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
    Scope<ContentBrowserPanel> m_ContentBrowserPanel;

    // Editor resources
    Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;
};
