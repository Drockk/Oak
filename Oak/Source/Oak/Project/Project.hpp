#pragma once

#include <string>
#include <filesystem>

#include "Oak/Core/Base.hpp"

namespace oak {
    namespace fs = std::filesystem;

    struct ProjectConfig
    {
        std::string name = "Untitled";

        fs::path startScene;

        fs::path assetDirectory;
        fs::path scriptModulePath;
    };

    class Project
    {
    public:
        static const fs::path& getProjectDirectory()
        {
            OAK_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->m_ProjectDirectory;
        }

        static fs::path getAssetDirectory()
        {
            OAK_CORE_ASSERT(s_ActiveProject);
            return getProjectDirectory() / s_ActiveProject->m_Config.assetDirectory;
        }

        // TODO: move to asset manager when we have one
        static fs::path getAssetFileSystemPath(const fs::path& path)
        {
            OAK_CORE_ASSERT(s_ActiveProject);
            return getAssetDirectory() / path;
        }

        ProjectConfig& getConfig() { return m_Config; }

        static oak::Ref<Project> getActive() { return s_ActiveProject; }

        static oak::Ref<Project> newProject();
        static oak::Ref<Project> load(const fs::path& path);
        static bool saveActive(const fs::path& path);
    private:
        ProjectConfig m_Config;
        fs::path m_ProjectDirectory;

        inline static oak::Ref<Project> s_ActiveProject;
    };
}
