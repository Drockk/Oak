include "./Thridparty/Premake/PremakeCustomization/solutionItems.lua"
include "dependencies.lua"

workspace "Oak"
    architecture "x86_64"
    startproject "OakEd"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    solution_items
    {
        ".editorconfig"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    group "Core"
        include "Oak"
        include "Oak-ScriptCore"
    group ""

    group "Dependencies"
        include "Thridparty/Premake"
        include "Oak/Thridparty/glad"
        include "Oak/Thridparty/glfw"
    group ""

    group "Examples"
        include "Sandbox"
    group ""

    group "Tools"
        "OakEd"
    group ""
