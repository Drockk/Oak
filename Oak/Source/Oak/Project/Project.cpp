#include "oakpch.hpp"
#include "Oak/Project/Project.hpp"

#include "Oak/Project/ProjectSerializer.hpp"

namespace oak
{
    Ref<Project> Project::newProject()
    {
        s_ActiveProject = createRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::load(const std::filesystem::path& path)
    {
        Ref<Project> project = createRef<Project>();

        ProjectSerializer serializer(project);
        if (serializer.deserialize(path))
        {
            project->m_ProjectDirectory = path.parent_path();
            s_ActiveProject = project;
            return s_ActiveProject;
        }

        return nullptr;
    }

    bool Project::saveActive(const std::filesystem::path& path)
    {
        ProjectSerializer serializer(s_ActiveProject);
        if (serializer.serialize(path))
        {
            s_ActiveProject->m_ProjectDirectory = path.parent_path();
            return true;
        }

        return false;
    }
}
