#include "oakpch.hpp"
#include "Oak/Scene/Entity.hpp"

namespace oak
{
    Entity::Entity(entt::entity t_handle, Scene* t_scene)
        : m_EntityHandle(t_handle), m_Scene(t_scene)
    {
    }
}
