-- Oak dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

Thirdparty = "%{wks.location}/Oak/Thridparty"

IncludeDir = {}
IncludeDir["stb_image"] = "%{Thirdparty}/stb_image"
IncludeDir["yaml_cpp"] = "%{Thirdparty}/yaml-cpp/include"
IncludeDir["Box2D"] = "%{Thirdparty}/Box2D/include"
IncludeDir["filewatch"] = "%{Thirdparty}/filewatch"
IncludeDir["GLFW"] = "%{Thirdparty}/GLFW/include"
IncludeDir["Glad"] = "%{Thirdparty}/Glad/include"
IncludeDir["ImGui"] = "%{Thirdparty}/imgui"
IncludeDir["ImGuizmo"] = "%{Thirdparty}/ImGuizmo"
IncludeDir["glm"] = "%{Thirdparty}/glm"
IncludeDir["entt"] = "%{Thirdparty}/entt/include"
IncludeDir["mono"] = "%{Thirdparty}/mono/include"
IncludeDir["shaderc"] = "%{Thirdparty}/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{Thirdparty}/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["msdfgen"] = "%{Thirdparty}/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{Thirdparty}/msdf-atlas-gen/msdf-atlas-gen"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["mono"] = "%{Thirdparty}/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"
