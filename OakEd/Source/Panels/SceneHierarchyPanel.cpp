#include "SceneHierarchyPanel.hpp"
#include <Oak/Scene/Components.hpp>

#include <Oak/Scripting/ScriptEngine.hpp>
#include <Oak/UI/UI.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

SceneHierarchyPanel::SceneHierarchyPanel(const oak::Ref<oak::Scene>& context)
{
    setContext(context);
}

void SceneHierarchyPanel::setContext(const oak::Ref<oak::Scene>& context)
{
    m_Context = context;
    m_SelectionContext = {};
}

void SceneHierarchyPanel::onImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    if (m_Context) {
        m_Context->m_Registry.each([&](auto entityID) {
            oak::Entity entity{ entityID , m_Context.get() };
            drawEntityNode(entity);
        });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            m_SelectionContext = {};
        }

        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow(0, 1)) {
            if (ImGui::MenuItem("Create Empty Entity")) {
                m_Context->createEntity("Empty Entity");
            }

            ImGui::EndPopup();
        }

    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (m_SelectionContext) {
        drawComponents(m_SelectionContext);
    }

    ImGui::End();
}

void SceneHierarchyPanel::setSelectedEntity(oak::Entity entity)
{
    m_SelectionContext = entity;
}

void SceneHierarchyPanel::drawEntityNode(oak::Entity entity)
{
    auto& tag = entity.getComponent<oak::TagComponent>().tag;

    ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    auto opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), flags, tag.c_str());
    if (ImGui::IsItemClicked()) {
        m_SelectionContext = entity;
    }

    auto entityDeleted{ false };
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete Entity")) {
            entityDeleted = true;
        }

        ImGui::EndPopup();
    }

    if (opened) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        auto opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(9817239), flags, tag.c_str());
        if (opened) {
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    if (entityDeleted) {
        m_Context->destroyEntity(entity);
        if (m_SelectionContext == entity) {
            m_SelectionContext = {};
        }
    }
}

static void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
    auto& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    auto lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

template<typename T, typename UIFunction>
static void drawComponent(const std::string& name, oak::Entity entity, UIFunction uiFunction)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

    if (entity.hasComponent<T>()) {
        auto& component = entity.getComponent<T>();
        auto contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        auto lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        auto open = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), treeNodeFlags, name.c_str());
        ImGui::PopStyleVar();
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
            ImGui::OpenPopup("ComponentSettings");
        }

        auto removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("Remove component")) {
                removeComponent = true;
            }

            ImGui::EndPopup();
        }

        if (open) {
            uiFunction(component);
            ImGui::TreePop();
        }

        if (removeComponent) {
            entity.removeComponent<T>();
        }
    }
}

void SceneHierarchyPanel::drawComponents(oak::Entity entity)
{
    if (entity.hasComponent<oak::TagComponent>()) {
        auto& tag = entity.getComponent<oak::TagComponent>().tag;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
            tag = std::string(buffer);
        }
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent")) {
        displayAddComponentEntry<oak::CameraComponent>("Camera");
        displayAddComponentEntry<oak::ScriptComponent>("Script");
        displayAddComponentEntry<oak::SpriteRendererComponent>("Sprite Renderer");
        displayAddComponentEntry<oak::CircleRendererComponent>("Circle Renderer");
        displayAddComponentEntry<oak::Rigidbody2DComponent>("Rigidbody 2D");
        displayAddComponentEntry<oak::BoxCollider2DComponent>("Box Collider 2D");
        displayAddComponentEntry<oak::CircleCollider2DComponent>("Circle Collider 2D");
        displayAddComponentEntry<oak::TextComponent>("Text Component");

        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    drawComponent<oak::TransformComponent>("Transform", entity, [](auto& component) {
        drawVec3Control("Translation", component.translation);
        auto rotation = glm::degrees(component.rotation);
        drawVec3Control("Rotation", rotation);
        component.rotation = glm::radians(rotation);
        drawVec3Control("Scale", component.scale, 1.0f);
    });

    drawComponent<oak::CameraComponent>("Camera", entity, [](auto& component) {
        auto& camera = component.camera;

        ImGui::Checkbox("Primary", &component.primary);

        const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
        const auto* currentProjectionTypeString = projectionTypeStrings[static_cast<int>(camera.getProjectionType())];
        if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
            for (auto i = 0; i < 2; i++) {
                auto isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
                    currentProjectionTypeString = projectionTypeStrings[i];
                    camera.setProjectionType(static_cast<oak::SceneCamera::ProjectionType>(i));
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        if (camera.getProjectionType() == oak::SceneCamera::ProjectionType::Perspective) {
            auto perspectiveVerticalFov = glm::degrees(camera.getPerspectiveVerticalFOV());
            if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov)) {
                camera.setPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));
            }

            auto perspectiveNear = camera.getPerspectiveNearClip();
            if (ImGui::DragFloat("Near", &perspectiveNear)) {
                camera.setPerspectiveNearClip(perspectiveNear);
            }

            auto perspectiveFar = camera.getPerspectiveFarClip();
            if (ImGui::DragFloat("Far", &perspectiveFar)) {
                camera.setPerspectiveFarClip(perspectiveFar);
            }
        }

        if (camera.getProjectionType() == oak::SceneCamera::ProjectionType::Orthographic) {
            auto orthoSize = camera.getOrthographicSize();
            if (ImGui::DragFloat("Size", &orthoSize)) {
                camera.setOrthographicSize(orthoSize);
            }

            auto orthoNear = camera.getOrthographicNearClip();
            if (ImGui::DragFloat("Near", &orthoNear)) {
                camera.setOrthographicNearClip(orthoNear);
            }

            auto orthoFar = camera.getOrthographicFarClip();
            if (ImGui::DragFloat("Far", &orthoFar)) {
                camera.setOrthographicFarClip(orthoFar);
            }

            ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
        }
    });

    drawComponent<oak::ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable {
        auto scriptClassExists = oak::ScriptEngine::entityClassExists(component.className);

        static char buffer[64];
        strcpy_s(buffer, sizeof(buffer), component.className.c_str());

        oak::ui::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

        if (ImGui::InputText("Class", buffer, sizeof(buffer))) {
            component.className = buffer;
            return;
        }

        // Fields
        auto sceneRunning = scene->isRunning();
        if (sceneRunning) {
            auto scriptInstance = oak::ScriptEngine::getEntityScriptInstance(entity.getUUID());
            if (scriptInstance) {
                const auto& fields = scriptInstance->getScriptClass()->getFields();
                for (const auto& [name, field] : fields) {
                    if (field.type == oak::ScriptFieldType::Float) {
                        auto data = scriptInstance->getFieldValue<float>(name);
                        if (ImGui::DragFloat(name.c_str(), &data)) {
                            scriptInstance->setFieldValue(name, data);
                        }
                    }
                }
            }
        }
        else {
            if (scriptClassExists) {
                auto entityClass = oak::ScriptEngine::getEntityClass(component.className);
                const auto& fields = entityClass->getFields();

                auto& entityFields = oak::ScriptEngine::getScriptFieldMap(entity);
                for (const auto& [name, field] : fields) {
                    // Field has been set in editor
                    if (entityFields.find(name) != entityFields.end()) {
                        auto& scriptField = entityFields.at(name);

                        // Display control to set it maybe
                        if (field.type == oak::ScriptFieldType::Float) {
                            auto data = scriptField.getValue<float>();
                            if (ImGui::DragFloat(name.c_str(), &data)) {
                                scriptField.setValue(data);
                            }
                        }
                    }
                    else {
                        // Display control to set it maybe
                        if (field.type == oak::ScriptFieldType::Float) {
                            auto data = 0.0f;
                            if (ImGui::DragFloat(name.c_str(), &data)) {
                                auto& fieldInstance = entityFields[name];
                                fieldInstance.field = field;
                                fieldInstance.setValue(data);
                            }
                        }
                    }
                }
            }
        }
    });

    drawComponent<oak::SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component) {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.color));

        ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
        if (ImGui::BeginDragDropTarget()) {
            if (const auto* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                auto path = static_cast<const wchar_t*>(payload->Data);
                std::filesystem::path texturePath(path);
                auto texture = oak::Texture2D::create(texturePath.string());
                if (texture->isLoaded()) {
                    component.texture = texture;
                }
                else {
                    OAK_LOG_CRITICAL("Could not load texture {0}", texturePath.filename().string());
                }
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
    });

    drawComponent<oak::CircleRendererComponent>("Circle Renderer", entity, [](auto& component) {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
        ImGui::DragFloat("Thickness", &component.thickness, 0.025f, 0.0f, 1.0f);
        ImGui::DragFloat("Fade", &component.fade, 0.00025f, 0.0f, 1.0f);
    });

    drawComponent<oak::Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component) {
        const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic"};
        const auto* currentBodyTypeString = bodyTypeStrings[static_cast<int>(component.type)];
        if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) {
            for (auto i = 0; i < 2; i++) {
                auto isSelected = currentBodyTypeString == bodyTypeStrings[i];
                if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
                    currentBodyTypeString = bodyTypeStrings[i];
                    component.type = static_cast<oak::Rigidbody2DComponent::BodyType>(i);
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
    });

    drawComponent<oak::BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component) {
        ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
        ImGui::DragFloat2("Size", glm::value_ptr(component.size));
        ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f);
    });

    drawComponent<oak::CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component) {
        ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
        ImGui::DragFloat("Radius", &component.radius);
        ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f);
    });

    drawComponent<oak::TextComponent>("Text Renderer", entity, [](auto& component) {
        ImGui::InputTextMultiline("Text String", &component.textString);
        ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
        ImGui::DragFloat("Kerning", &component.kerning, 0.025f);
        ImGui::DragFloat("Line Spacing", &component.lineSpacing, 0.025f);
    });

}

template<typename T>
void SceneHierarchyPanel::displayAddComponentEntry(const std::string& entryName)
{
    if (!m_SelectionContext.hasComponent<T>()) {
        if (ImGui::MenuItem(entryName.c_str())) {
            m_SelectionContext.addComponent<T>();
            ImGui::CloseCurrentPopup();
        }
    }
}
