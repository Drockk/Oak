#include "oakpch.hpp"
#include "Project.hpp"

#include "ProjectSerializer.hpp"

namespace oak {
    Ref<Project> Project::newProject()
    {
        s_ActiveProject = createRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::load(const fs::path& path)
    {
        auto project = createRef<Project>();

        if (ProjectSerializer serializer(project); serializer.deserialize(path)) {
            project->m_ProjectDirectory = path.parent_path();
            s_ActiveProject = project;
            return s_ActiveProject;
        }

        return nullptr;
    }

    bool Project::saveActive(const fs::path& path)
    {
        if (ProjectSerializer serializer(s_ActiveProject); serializer.serialize(path)) {
            s_ActiveProject->m_ProjectDirectory = path.parent_path();
            return true;
        }

        return false;
    }
}
