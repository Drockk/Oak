workspace "Oak"
    architecture("x86_64")

    configurations
    {
        "Debug",
        "Release"
    }

    project "Oak"
        kind "ConsoleApp"
        language "C++"
        targetdir "bin/%{cfg.buildcfg}"

        files
        {
            "**.hpp",
            "**.cpp"
        }

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
