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
        "%{IncludeDir.entt}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.oak}",
        "%{wks.location}/Oak/Thirdparty",
    }

    links
    {
        "Oak"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "OAK_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "OAK_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "OAK_DIST"
        runtime "Release"
        optimize "on"
