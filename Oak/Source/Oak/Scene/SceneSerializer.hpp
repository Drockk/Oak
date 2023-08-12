#pragma once

#include "Scene.hpp"

namespace oak {
    class SceneSerializer
    {
    public:
        SceneSerializer(const oak::Ref<oak::Scene>& scene);

        void serialize(const std::string& filepath);
        void serializeRuntime(const std::string& filepath);

        bool deserialize(const std::string& filepath);
        bool deserializeRuntime(const std::string& filepath);

    private:
        Ref<Scene> m_Scene;
    };
}
