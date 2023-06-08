file(GLOB_RECURSE CPP_FILES Source/**.cpp)
file(GLOB_RECURSE HEADER_FILES Source/**.hpp)

CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage("gh:glfw/glfw#3.3.8")

add_library(Oak STATIC ${CPP_FILES} ${HEADER_FILES})

set_target_properties(Oak PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(Oak spdlog glfw3)

set_property(TARGET Oak PROPERTY CXX_STANDARD 23)
set_property(TARGET Oak PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET Oak PROPERTY CXX_EXTENSIONS Off)

set(OAK_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Oak/Source
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

target_include_directories(Oak PRIVATE ${OAK_INCLUDE_DIRS})
