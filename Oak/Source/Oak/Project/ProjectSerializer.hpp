#pragma once

#include "Oak/Project/Project.hpp"

namespace oak
{
    class ProjectSerializer
    {
    public:
        ProjectSerializer(Ref<Project> project);

        bool serialize(const std::filesystem::path& filepath);
        bool deserialize(const std::filesystem::path& filepath);
    private:
        Ref<Project> m_Project;
    };
}
