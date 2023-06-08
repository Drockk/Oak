project "Oak"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")

    pchheader "oakpch.hpp"
    pchsource "Source/oakpch.cpp"

    files
    {
        "Source/**.hpp",
        "Source/**.cpp"
    }

    includedirs
    {
        "Source"
    }

    externalincludedirs
    {
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glfw}"
    }

    links
    {
        "GLFW"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "VI_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "VI_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "VI_RELEASE"
        runtime "Release"
        optimize "On"
