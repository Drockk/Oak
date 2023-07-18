#pragma once

#include <string>
#include <filesystem>

#include "Oak/Core/Base.hpp"

namespace oak
{
    struct ProjectConfig
    {
        std::string name = "Untitled";

        std::filesystem::path startScene;

        std::filesystem::path assetDirectory;
        std::filesystem::path scriptModulePath;
    };

    class Project
    {
    public:
        static const std::filesystem::path& getProjectDirectory()
        {
            OAK_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->m_ProjectDirectory;
        }

        static std::filesystem::path getAssetDirectory()
        {
            OAK_CORE_ASSERT(s_ActiveProject);
            return getProjectDirectory() / s_ActiveProject->m_Config.assetDirectory;
        }

        // TODO: move to asset manager when we have one
        static std::filesystem::path getAssetFileSystemPath(const std::filesystem::path& path)
        {
            OAK_CORE_ASSERT(s_ActiveProject);
            return getAssetDirectory() / path;
        }

        ProjectConfig& getConfig() { return m_Config; }

        static Ref<Project> getActive() { return s_ActiveProject; }

        static Ref<Project> newProject();
        static Ref<Project> load(const std::filesystem::path& path);
        static bool saveActive(const std::filesystem::path& path);
    private:
        ProjectConfig m_Config;
        std::filesystem::path m_ProjectDirectory;

        inline static Ref<Project> s_ActiveProject;
    };
}
