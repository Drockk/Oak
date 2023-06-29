#pragma once

#include "Oak/Core/Base.hpp"
#include "Oak/Scene/Scene.hpp"
#include "Oak/Scene/Entity.hpp"

class SceneHierarchyPanel
{
public:
SceneHierarchyPanel() = default;
SceneHierarchyPanel(const oak::Ref<oak::Scene>& t_scene);
~SceneHierarchyPanel() = default;

void setContext(const oak::Ref<oak::Scene>& t_scene);

void onImGuiRender();

oak::Entity getSelectedEntity() const { return m_SelectionContext; }
void setSelectedEntity(oak::Entity entity);

private:
template<typename T>
void displayAddComponentEntry(const std::string& entryName);

void drawEntityNode(oak::Entity t_entity);
void drawComponents(oak::Entity t_entity);

oak::Ref<oak::Scene> m_Context;
oak::Entity m_SelectionContext;
};
