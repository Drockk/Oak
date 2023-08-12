#pragma once

#include "Project.hpp"

namespace oak {
    namespace fs = std::filesystem;

    class ProjectSerializer
    {
    public:
        ProjectSerializer(Ref<Project> project);

        bool serialize(const fs::path& filepath);
        bool deserialize(const fs::path& filepath);

    private:
        Ref<oak::Project> m_Project;
    };
}
