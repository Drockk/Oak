#pragma once

#include "Oak/Renderer/Texture.hpp"

#include <filesystem>

class ContentBrowserPanel
{
public:
    ContentBrowserPanel();

    void onImGuiRender();

private:
    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    oak::Ref<oak::Texture2D> m_DirectoryIcon;
    oak::Ref<oak::Texture2D> m_FileIcon;
};
