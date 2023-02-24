workspace "Oak"
    architecture("x86_64")

    configurations
    {
        "Debug",
        "Release"
    }

    group "Dependencies"
        include "thirdparty/glfw"
        include "thirdparty/glad"
    group ""

    include "Oak"
