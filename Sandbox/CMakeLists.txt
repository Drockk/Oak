file(GLOB_RECURSE CPP_FILES Source/**.cpp)
file(GLOB_RECURSE HEADER_FILES Source/*.hpp)

add_executable (Sandbox ${CPP_FILES} ${HEADER_FILES})

target_include_directories(Sandbox PRIVATE ${OAK_INCLUDE_DIRS})

target_link_libraries(Sandbox Oak glfw3)

set_property(TARGET Sandbox PROPERTY CXX_STANDARD 23)
set_property(TARGET Sandbox PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET Sandbox PROPERTY CXX_EXTENSIONS Off)
