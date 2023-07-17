#include "oakpch.hpp"
#include "Oak/Scene/Scene.hpp"
#include "Oak/Scene/Entity.hpp"

#include "Oak/Scene/Components.hpp"
#include "Oak/Scene/ScriptableEntity.hpp"
#include "Oak/Scripting/ScriptEngine.hpp"
#include "Oak/Renderer/Renderer2D.hpp"
#include "Oak/Physics/Physics2D.hpp"

#include <glm/glm.hpp>

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace oak
{
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
        delete m_PhysicsWorld;
    }

    template<typename... Component>
    static void copyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        ([&]()
            {
                auto view = src.view<Component>();
                for (auto srcEntity : view)
                {
                    entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

                    auto& srcComponent = src.get<Component>(srcEntity);
                    dst.emplace_or_replace<Component>(dstEntity, srcComponent);
                }
            }(), ...);
    }

    template<typename... Component>
    static void copyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template<typename... Component>
    static void copyComponentIfExists(Entity dst, Entity src)
    {
        ([&]()
            {
                if (src.hasComponent<Component>())
                    dst.AddOrReplaceComponent<Component>(src.getComponent<Component>());
            }(), ...);
    }

    template<typename... Component>
    static void copyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        CopyComponentIfExists<Component...>(dst, src);
    }

    Ref<Scene> Scene::copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = createRef<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;

        // Create entities in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).id;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).tag;
            Entity newEntity = newScene->createEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        // Copy components (except IDComponent and TagComponent)
        copyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }

    Entity Scene::createEntity(const std::string& name)
    {
        return createEntityWithUUID(UUID(), name);
    }

    Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name)
    {
        Entity entity = { m_Registry.create(), this };
        entity.addComponent<IDComponent>(uuid);
        entity.addComponent<TransformComponent>();
        auto& tag = entity.addComponent<TagComponent>();
        tag.tag = name.empty() ? "Entity" : name;

        m_EntityMap[uuid] = entity;

        return entity;
    }

    void Scene::destroyEntity(Entity entity)
    {
        m_EntityMap.erase(entity.getUUID());
        m_Registry.destroy(entity);
    }

    void Scene::onRuntimeStart()
    {
        m_IsRunning = true;

        onPhysics2DStart();

        // Scripting
        {
            ScriptEngine::onRuntimeStart(this);
            // Instantiate all script entities

            auto view = m_Registry.view<ScriptComponent>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                ScriptEngine::onCreateEntity(entity);
            }
        }
    }

    void Scene::onRuntimeStop()
    {
        m_IsRunning = false;

        onPhysics2DStop();

        ScriptEngine::onRuntimeStop();
    }

    void Scene::onSimulationStart()
    {
        onPhysics2DStart();
    }

    void Scene::onSimulationStop()
    {
        onPhysics2DStop();
    }

    void Scene::onUpdateRuntime(Timestep ts)
    {
        if (!m_IsPaused || m_StepFrames-- > 0)
        {
            // Update scripts
            {
                // C# Entity OnUpdate
                auto view = m_Registry.view<ScriptComponent>();
                for (auto e : view)
                {
                    Entity entity = { e, this };
                    ScriptEngine::onUpdateEntity(entity, ts);
                }

                m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
                    {
                        // TODO: Move to Scene::OnScenePlay
                        if (!nsc.Instance)
                        {
                            nsc.Instance = nsc.InstantiateScript();
                            nsc.Instance->m_Entity = Entity{ entity, this };
                            nsc.Instance->OnCreate();
                        }

                        nsc.Instance->OnUpdate(ts);
                    });
            }

            // Physics
            {
                const int32_t velocityIterations = 6;
                const int32_t positionIterations = 2;
                m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

                // Retrieve transform from Box2D
                auto view = m_Registry.view<Rigidbody2DComponent>();
                for (auto e : view)
                {
                    Entity entity = { e, this };
                    auto& transform = entity.getComponent<TransformComponent>();
                    auto& rb2d = entity.getComponent<Rigidbody2DComponent>();

                    b2Body* body = (b2Body*)rb2d.runtimeBody;

                    const auto& position = body->GetPosition();
                    transform.translation.x = position.x;
                    transform.translation.y = position.y;
                    transform.rotation.z = body->GetAngle();
                }
            }
        }

        // Render 2D
        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

                if (camera.primary)
                {
                    mainCamera = &camera.camera;
                    cameraTransform = transform.getTransform();
                    break;
                }
            }
        }

        if (mainCamera)
        {
            Renderer2D::beginScene(*mainCamera, cameraTransform);

            // Draw sprites
            {
                auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
                for (auto entity : group)
                {
                    auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                    Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
                }
            }

            // Draw circles
            {
                auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
                for (auto entity : view)
                {
                    auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                    Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, (int)entity);
                }
            }

            // Draw text
            {
                auto view = m_Registry.view<TransformComponent, TextComponent>();
                for (auto entity : view)
                {
                    auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

                    Renderer2D::drawString(text.textString, transform.getTransform(), text, (int)entity);
                }
            }

            Renderer2D::endScene();
        }

    }

    void Scene::onUpdateSimulation(Timestep ts, EditorCamera& camera)
    {
        if (!m_IsPaused || m_StepFrames-- > 0)
        {
            // Physics
            {
                const int32_t velocityIterations = 6;
                const int32_t positionIterations = 2;
                m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

                // Retrieve transform from Box2D
                auto view = m_Registry.view<Rigidbody2DComponent>();
                for (auto e : view)
                {
                    Entity entity = { e, this };
                    auto& transform = entity.getComponent<TransformComponent>();
                    auto& rb2d = entity.getComponent<Rigidbody2DComponent>();

                    b2Body* body = (b2Body*)rb2d.runtimeBody;
                    const auto& position = body->GetPosition();
                    transform.translation.x = position.x;
                    transform.translation.y = position.y;
                    transform.rotation.z = body->GetAngle();
                }
            }
        }

        // Render
        renderScene(camera);
    }

    void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
    {
        // Render
        renderScene(camera);
    }

    void Scene::onViewportResize(uint32_t width, uint32_t height)
    {
        if (m_ViewportWidth == width && m_ViewportHeight == height)
            return;

        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // Resize our non-FixedAspectRatio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.fixedAspectRatio)
                cameraComponent.camera.setViewportSize(width, height);
        }
    }

    Entity Scene::getPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            const auto& camera = view.get<CameraComponent>(entity);
            if (camera.primary)
                return Entity{ entity, this };
        }
        return {};
    }

    void Scene::step(int frames)
    {
        m_StepFrames = frames;
    }

    Entity Scene::duplicateEntity(Entity entity)
    {
        // Copy name because we're going to modify component data structure
        std::string name = entity.getName();
        Entity newEntity = createEntity(name);
        copyComponentIfExists(AllComponents{}, newEntity, entity);
        return newEntity;
    }

    Entity Scene::findEntityByName(std::string_view name)
    {
        auto view = m_Registry.view<TagComponent>();
        for (auto entity : view)
        {
            const TagComponent& tc = view.get<TagComponent>(entity);
            if (tc.tag == name)
                return Entity{ entity, this };
        }
        return {};
    }

    Entity Scene::getEntityByUUID(UUID uuid)
    {
        // TODO(Yan): Maybe should be assert
        if (m_EntityMap.find(uuid) != m_EntityMap.end())
            return { m_EntityMap.at(uuid), this };

        return {};
    }

    void Scene::onPhysics2DStart()
    {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.getComponent<TransformComponent>();
            auto& rb2d = entity.getComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef;
            bodyDef.type = utils::rigidbody2DTypeToBox2DBody(rb2d.type);
            bodyDef.position.Set(transform.translation.x, transform.translation.y);
            bodyDef.angle = transform.rotation.z;

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.fixedRotation);
            rb2d.runtimeBody = body;

            if (entity.hasComponent<BoxCollider2DComponent>())
            {
                auto& bc2d = entity.getComponent<BoxCollider2DComponent>();

                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.size.x * transform.scale.x, bc2d.size.y * transform.scale.y, b2Vec2(bc2d.offset.x, bc2d.offset.y), 0.0f);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.density;
                fixtureDef.friction = bc2d.friction;
                fixtureDef.restitution = bc2d.restitution;
                fixtureDef.restitutionThreshold = bc2d.restitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }

            if (entity.hasComponent<CircleCollider2DComponent>())
            {
                auto& cc2d = entity.getComponent<CircleCollider2DComponent>();

                b2CircleShape circleShape;
                circleShape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
                circleShape.m_radius = transform.scale.x * cc2d.radius;

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &circleShape;
                fixtureDef.density = cc2d.density;
                fixtureDef.friction = cc2d.friction;
                fixtureDef.restitution = cc2d.restitution;
                fixtureDef.restitutionThreshold = cc2d.restitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    void Scene::onPhysics2DStop()
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }

    void Scene::renderScene(EditorCamera& camera)
    {
        Renderer2D::beginScene(camera);

        // Draw sprites
        {
            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
            }
        }

        // Draw circles
        {
            auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view)
            {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, (int)entity);
            }
        }

        // Draw text
        {
            auto view = m_Registry.view<TransformComponent, TextComponent>();
            for (auto entity : view)
            {
                auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

                Renderer2D::drawString(text.textString, transform.getTransform(), text, (int)entity);
            }
        }

        Renderer2D::endScene();
    }

    template<typename T>
    void Scene::onComponentAdded(Entity entity, T& component)
    {
        static_assert(sizeof(T) == 0);
    }

    template<>
    void Scene::onComponentAdded<IDComponent>(Entity entity, IDComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
    {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.camera.setViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template<>
    void Scene::onComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
    {
    }

    template<>
    void Scene::onComponentAdded<TextComponent>(Entity entity, TextComponent& component)
    {
    }
}
