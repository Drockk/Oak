#include "oakpch.hpp"
#include "Entity.hpp"

namespace oak {
    Entity::Entity(entt::entity handle, Scene* scene): m_EntityHandle(handle), m_Scene(scene)
    {
    }
}
