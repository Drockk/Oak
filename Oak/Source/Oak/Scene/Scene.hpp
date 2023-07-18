#pragma once

#include "Oak/Core/Timestep.hpp"
#include "Oak/Core/UUID.hpp"
#include "Oak/Renderer/EditorCamera.hpp"

#include "entt.hpp"

class b2World;
class SceneHierarchyPanel;

namespace oak
{
    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        static Ref<Scene> copy(Ref<Scene> other);

        Entity createEntity(const std::string& name = std::string());
        Entity createEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void destroyEntity(Entity entity);

        void onRuntimeStart();
        void onRuntimeStop();

        void onSimulationStart();
        void onSimulationStop();

        void onUpdateRuntime(Timestep ts);
        void onUpdateSimulation(Timestep ts, EditorCamera& camera);
        void onUpdateEditor(Timestep ts, EditorCamera& camera);
        void onViewportResize(uint32_t width, uint32_t height);

        Entity duplicateEntity(Entity entity);

        Entity findEntityByName(std::string_view name);
        Entity getEntityByUUID(UUID uuid);

        Entity getPrimaryCameraEntity();

        bool isRunning() const { return m_IsRunning; }
        bool isPaused() const { return m_IsPaused; }

        void setPaused(bool paused) { m_IsPaused = paused; }

        void step(int frames = 1);

        template<typename... Components>
        auto getAllEntitiesWith()
        {
            return m_Registry.view<Components...>();
        }
    private:
        template<typename T>
        void onComponentAdded(Entity entity, T& component);

        void onPhysics2DStart();
        void onPhysics2DStop();

        void renderScene(EditorCamera& camera);
    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        bool m_IsRunning = false;
        bool m_IsPaused = false;
        int m_StepFrames = 0;

        b2World* m_PhysicsWorld = nullptr;

        std::unordered_map<UUID, entt::entity> m_EntityMap;

        friend class Entity;
        friend class SceneSerializer;
        friend class ::SceneHierarchyPanel;
    };
}
