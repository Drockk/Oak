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

    void onUpdate(oak::Timestep t_timestep) override;
    void onImGuiRender() override;
    void onEvent(oak::Event& t_event) override;

private:
    bool onKeyPressed(oak::KeyPressedEvent& t_event);
    bool onMouseButtonPressed(oak::MouseButtonPressedEvent& t_event);

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

    void serializeScene(oak::Ref<oak::Scene> t_scene, const std::filesystem::path& t_path);

    void onScenePlay();
    void onSceneSimulate();
    void onSceneStop();
    void onScenePause();

    void onDuplicateEntity();

    // UI Panels
    void uiMenuBar();
    void uiSettings();
    void uiStatistics();
    void uiToolbar();
    void uiViewport();

    // Temp
    oak::Ref<oak::Framebuffer> m_Framebuffer;
    oak::Ref<oak::Scene> m_ActiveScene;
    oak::Ref<oak::Scene> m_EditorScene;
    std::filesystem::path m_EditorScenePath;

    oak::Entity m_HoveredEntity;

    oak::EditorCamera m_EditorCamera;

    bool m_ViewportFocused{ false };
    bool m_ViewportHovered{ false };
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    glm::vec2 m_ViewportBounds[2] = {};

    int m_GizmoType{ -1 };

    bool m_ShowPhysicsColliders{ false };

    enum class SceneState
    {
        Edit = 0, Play = 1, Simulate = 2
    };
    SceneState m_SceneState{ SceneState::Edit };

    // Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    oak::Scope<ContentBrowserPanel> m_ContentBrowserPanel;

    // Editor resources
    oak::Ref<oak::Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;
};
