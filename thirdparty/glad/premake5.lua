project "GLAD"
    kind "StaticLib"
    language "C"

    targetdir "bin/%{cfg.buildcfg}"

    files
    {
        "include/**.h",
        "src/gl.c"
    }

    includedirs
    {
        "include"
    }
