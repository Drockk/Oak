#pragma once

#include "Oak/Core/UUID.hpp"
#include "Oak/Renderer/Font.hpp"
#include "Oak/Renderer/Texture.hpp"
#include "Oak/Scene/SceneCamera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace oak
{
    struct IDComponent
    {
        UUID id;

        IDComponent() = default;
        IDComponent(UUID t_id): id{ t_id } {};
        IDComponent(const IDComponent&) = default;
    };

    struct TagComponent
    {
        std::string tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& t_tag)
            : tag(t_tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& t_translation)
            : translation(t_translation) {}

        glm::mat4 getTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(rotation));

            return glm::translate(glm::mat4(1.0f), translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), scale);
        }
    };

    struct SpriteRendererComponent
    {
        glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        Ref<Texture2D> texture;
        float tilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& t_color)
            : color(t_color) {}
    };

    struct CircleRendererComponent
    {
        glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float thickness = 1.0f;
        float fade = 0.005f;

        CircleRendererComponent() = default;
        CircleRendererComponent(const CircleRendererComponent&) = default;
    };

    struct CameraComponent
    {
        SceneCamera camera;
        bool primary = true; // TODO: think about moving to Scene
        bool fixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct ScriptComponent
    {
        std::string className;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
    };

    // Forward declaration
    class ScriptableEntity;

    struct NativeScriptComponent
    {
        ScriptableEntity* instance = nullptr;

        ScriptableEntity* (*instantiateScript)();
        void (*destroyScript)(NativeScriptComponent*);

        template<typename T>
        void Bind()
        {
            instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
            destroyScript = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
        }
    };

    // Physics

    struct Rigidbody2DComponent
    {
        enum class BodyType { Static = 0, Dynamic, Kinematic };
        BodyType type = BodyType::Static;
        bool fixedRotation = false;

        // Storage for runtime
        void* runtimeBody = nullptr;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };

    struct BoxCollider2DComponent
    {
        glm::vec2 offset = { 0.0f, 0.0f };
        glm::vec2 size = { 0.5f, 0.5f };

        // TODO: move into physics material in the future maybe
        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitutionThreshold = 0.5f;

        // Storage for runtime
        void* runtimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };

    struct CircleCollider2DComponent
    {
        glm::vec2 offset = { 0.0f, 0.0f };
        float radius = 0.5f;

        // TODO: move into physics material in the future maybe
        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitutionThreshold = 0.5f;

        // Storage for runtime
        void* runtimeFixture = nullptr;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
    };

    struct TextComponent
    {
        std::string textString;
        Ref<Font> fontAsset = Font::getDefault();
        glm::vec4 color{ 1.0f };
        float kerning = 0.0f;
        float lineSpacing = 0.0f;
    };

    template<typename... Component>
    struct ComponentGroup
    {
    };

    using AllComponents =
        ComponentGroup<TransformComponent, SpriteRendererComponent,
        CircleRendererComponent, CameraComponent, ScriptComponent,
        NativeScriptComponent, Rigidbody2DComponent, BoxCollider2DComponent,
        CircleCollider2DComponent, TextComponent>;
}
