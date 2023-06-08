include "dependencies.lua"

workspace "Oak"
    architecture "x86_64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    group "Core"
        include "Oak"
    group ""

    group "Dependencies"
        include "Oak/Thridparty/glfw"
    group ""

    group "Examples"
        include "Sandbox"
    group ""
