project "Oak"
        kind "ConsoleApp"
        language "C++"
        targetdir "bin/%{cfg.buildcfg}"

        files
        {
            "source/**.hpp",
            "source/**.cpp"
        }

        includedirs
        {
            "source",
            "%{wks.location}/thirdparty/eventpp/include",
            "%{wks.location}/thirdparty/glfw/include",
            "%{wks.location}/thirdparty/glad/include"
        }

        links
        {
            "GLFW",
            "GLAD",
            "opengl32"
        }

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
