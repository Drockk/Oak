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

    Ref<Texture2D> m_DirectoryIcon;
    Ref<Texture2D> m_FileIcon;
};
