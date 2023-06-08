project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")

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
        "%{IncludeDir.oak}",
        "%{IncludeDir.spdlog}"
    }

    links
    {
        "Oak"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "VI_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "VI_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "VI_RELEASE"
        runtime "Release"
        optimize "on"
