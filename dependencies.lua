-- Oak dependencies

VULKAND_SDK = os.getenv("VULKAN_SDK")

ThridpartyDir = "%{wks.location}/Oak/Thridparty"

IncludeDir = {}
IncludeDir["Box2D"] = "%{ThridpartyDir}/Box2D/include"
IncludeDir["entt"] = "%{ThridpartyDir}/entt/include"
IncludeDir["filewatch"] = "%{ThridpartyDir}/filewatch"
IncludeDir["glad"] = "%{ThridpartyDir}/glad/include"
IncludeDir["glfw"] = "%{ThridpartyDir}/glfw/include"
IncludeDir["glm"] = "%{ThridpartyDir}/glm"
IncludeDir["ImGui"] = "%{ThridpartyDir}/ImGui"
IncludeDir["ImGuizmo"] = "%{ThridpartyDir}/ImGuizmo"
IncludeDir["mono"] = "%{ThridpartyDir}/mono/include"
IncludeDir["msdfgen"] = "%{ThridpartyDir}/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{ThridpartyDir}/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["shaderc"] = "%{ThridpartyDir}/shaderc/include"
IncludeDir["spdlog"] = "%{ThridpartyDir}/spdlog/include"
IncludeDir["SPIRV_Cross"] = "%{ThridpartyDir}/SPIRV-Cross"
IncludeDir["stb_image"] = "%{ThridpartyDir}/stb_image"
IncludeDir["yaml_cpp"] = "%{ThridpartyDir}/yaml-cpp/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
