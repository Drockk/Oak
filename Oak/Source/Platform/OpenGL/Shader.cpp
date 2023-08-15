#include "oakpch.hpp"
#include "Platform/OpenGL/Shader.hpp"
#include "Oak/Core/Timer.hpp"

#include <fstream>
#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace opengl {
    namespace utils {
        namespace fs = std::filesystem;

        constexpr static GLenum shaderTypeFromString(const std::string_view& t_type)
        {
            if (t_type == "vertex") {
                return GL_VERTEX_SHADER;
            }
            if (t_type == "fragment" || t_type == "pixel") {
                return GL_FRAGMENT_SHADER;
            }

            OAK_CORE_ASSERT(false, "Unknown shader type!");
            return 0;
        }

        constexpr static shaderc_shader_kind glShaderStageToShaderC(GLenum t_stage)
        {
            switch (t_stage){
            case GL_VERTEX_SHADER:
                return shaderc_glsl_vertex_shader;
            case GL_FRAGMENT_SHADER:
                return shaderc_glsl_fragment_shader;
            }

            OAK_CORE_ASSERT(false);
            return static_cast<shaderc_shader_kind>(0);
        }

        constexpr static const char* glShaderStageToString(GLenum stage)
        {
            switch (stage) {
            case GL_VERTEX_SHADER:
                return "GL_VERTEX_SHADER";
            case GL_FRAGMENT_SHADER:
                return "GL_FRAGMENT_SHADER";
            }

            OAK_CORE_ASSERT(false);
            return nullptr;
        }

        constexpr static const char* getCacheDirectory()
        {
            return "assets/cache/shader/opengl";
        }

        static void createCacheDirectoryIfNeeded()
        {
            auto cacheDirectory = getCacheDirectory();
            if (!fs::exists(cacheDirectory)) {
                fs::create_directories(cacheDirectory);
            }
        }

        static const char* glShaderStageCachedOpenGLFileExtension(uint32_t stage)
        {
            switch (stage) {
                case GL_VERTEX_SHADER:
                    return ".cached_opengl.vert";
                case GL_FRAGMENT_SHADER:
                    return ".cached_opengl.frag";
            }

            OAK_CORE_ASSERT(false);
            return nullptr;
        }

        static const char* glShaderStageCachedVulkanFileExtension(uint32_t stage)
        {
            switch (stage) {
            case GL_VERTEX_SHADER:
                return ".cached_vulkan.vert";
            case GL_FRAGMENT_SHADER:
                return ".cached_vulkan.frag";
            }

            OAK_CORE_ASSERT(false);
            return nullptr;
        }
    }

    Shader::Shader(const std::string& filepath): m_FilePath(filepath)
    {
        OAK_PROFILE_FUNCTION();

        utils::createCacheDirectoryIfNeeded();

        auto source = readFile(filepath);
        auto shaderSources = preProcess(source);

        {
            oak::Timer timer;
            compileOrGetVulkanBinaries(shaderSources);
            compileOrGetOpenGLBinaries();
            createProgram();
            OAK_LOG_CORE_WARN("Shader creation took {} ms", timer.elapsedMillis());
        }

        // Extract name from filepath
        m_Name = utils::fs::path(filepath).stem().string();
    }

    Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc): m_Name(name)
    {
        OAK_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;

        compileOrGetVulkanBinaries(sources);
        compileOrGetOpenGLBinaries();
        createProgram();
    }

    Shader::~Shader()
    {
        OAK_PROFILE_FUNCTION();

        glDeleteProgram(m_RendererID);
    }

    std::string Shader::readFile(const std::string& t_filepath)
    {
        OAK_PROFILE_FUNCTION();
        std::ifstream file(t_filepath, std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error(std::format("Cannot open shader file: {}", t_filepath));
        }

        auto fileSize = utils::fs::file_size(t_filepath);

        if (fileSize == 0) {
            throw std::runtime_error(std::format("{} size is 0", t_filepath));
        }

        std::string result{};
        result.resize(fileSize);

        if (!file.read(result.data(), result.size())) {
            throw std::runtime_error(t_filepath + ": " + std::strerror(errno));
        }

        return result;
    }

    std::unordered_map<GLenum, std::string> Shader::preProcess(const std::string& source)
    {
        OAK_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        auto typeTokenLength = strlen(typeToken);
        auto pos = source.find(typeToken, 0); //Start of shader type declaration line
        while (pos != std::string::npos) {
            auto eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
            OAK_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            auto begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
            auto type = source.substr(begin, eol - begin);
            OAK_CORE_ASSERT(utils::shaderTypeFromString(type), "Invalid shader type specified");

            auto nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
            OAK_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

            shaderSources[utils::shaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    void Shader::compileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        auto program = glCreateProgram();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        if constexpr (const auto optimize = true; optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_performance);
        }

        std::filesystem::path cacheDirectory = utils::getCacheDirectory();

        auto& shaderData = m_VulkanSPIRV;
        shaderData.clear();
        for (auto&& [stage, source] : shaderSources) {
            std::filesystem::path shaderFilePath = m_FilePath;
            std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + utils::glShaderStageCachedVulkanFileExtension(stage));

            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (in.is_open()) {
                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto& data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                in.read((char*)data.data(), size);
            }
            else {
                auto module = compiler.CompileGlslToSpv(source, utils::glShaderStageToShaderC(stage), m_FilePath.c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                    OAK_LOG_CORE_ERROR(module.GetErrorMessage());
                    OAK_CORE_ASSERT(false);
                }

                shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

                std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
                if (out.is_open()) {
                    auto& data = shaderData[stage];
                    out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                    out.flush();
                    out.close();
                }
            }
        }

        for (auto&& [stage, data]: shaderData) {
            reflect(stage, data);
        }
    }

    void Shader::compileOrGetOpenGLBinaries()
    {
        auto& shaderData = m_OpenGLSPIRV;

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        
        if constexpr (const auto optimize = false; optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_performance);
        }

        std::filesystem::path cacheDirectory = utils::getCacheDirectory();

        shaderData.clear();
        m_OpenGLSourceCode.clear();
        for (auto&& [stage, spirv] : m_VulkanSPIRV) {
            std::filesystem::path shaderFilePath = m_FilePath;
            auto cachedPath = cacheDirectory / (shaderFilePath.filename().string() + utils::glShaderStageCachedOpenGLFileExtension(stage));

            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (in.is_open()) {
                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto& data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                in.read((char*)data.data(), size);
            }
            else {
                spirv_cross::CompilerGLSL glslCompiler(spirv);
                m_OpenGLSourceCode[stage] = glslCompiler.compile();
                auto& source = m_OpenGLSourceCode[stage];

                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, utils::glShaderStageToShaderC(stage), m_FilePath.c_str());
                if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                    OAK_LOG_CORE_ERROR(module.GetErrorMessage());
                    OAK_CORE_ASSERT(false);
                }

                shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

                std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
                if (out.is_open()) {
                    auto& data = shaderData[stage];
                    out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                    out.flush();
                    out.close();
                }
            }
        }
    }

    void Shader::createProgram()
    {
        auto program = glCreateProgram();

        std::vector<GLuint> shaderIDs;
        for (auto&& [stage, spirv] : m_OpenGLSPIRV) {
            auto shaderID = shaderIDs.emplace_back(glCreateShader(stage));
            glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), static_cast<GLsizei>(spirv.size() * sizeof(uint32_t)));
            glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
            glAttachShader(program, shaderID);
        }

        glLinkProgram(program);

        GLint isLinked;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
            OAK_LOG_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

            glDeleteProgram(program);

            for (auto id : shaderIDs) {
                glDeleteShader(id);
            }
        }

        for (auto id : shaderIDs) {
            glDetachShader(program, id);
            glDeleteShader(id);
        }

        m_RendererID = program;
    }

    void Shader::reflect(GLenum stage, const std::vector<uint32_t>& shaderData) {
        spirv_cross::Compiler compiler(shaderData);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        OAK_LOG_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", utils::glShaderStageToString(stage), m_FilePath);
        OAK_LOG_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
        OAK_LOG_CORE_TRACE("    {0} resources", resources.sampled_images.size());

        OAK_LOG_CORE_TRACE("Uniform buffers:");
        for (const auto& resource : resources.uniform_buffers) {
            const auto& bufferType = compiler.get_type(resource.base_type_id);
            auto bufferSize = compiler.get_declared_struct_size(bufferType);
            auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            auto memberCount = bufferType.member_types.size();

            OAK_LOG_CORE_TRACE("  {0}", resource.name);
            OAK_LOG_CORE_TRACE("    Size = {0}", bufferSize);
            OAK_LOG_CORE_TRACE("    Binding = {0}", binding);
            OAK_LOG_CORE_TRACE("    Members = {0}", memberCount);
        }
    }

    void Shader::bind() const
    {
        OAK_PROFILE_FUNCTION();

        glUseProgram(m_RendererID);
    }

    void Shader::unbind() const
    {
        OAK_PROFILE_FUNCTION();

        glUseProgram(0);
    }

    void Shader::setInt(const std::string& name, int value)
    {
        OAK_PROFILE_FUNCTION();

        uploadUniformInt(name, value);
    }

    void Shader::setIntArray(const std::string& name, int* values, uint32_t count)
    {
        uploadUniformIntArray(name, values, count);
    }

    void Shader::setFloat(const std::string& name, float value)
    {
        OAK_PROFILE_FUNCTION();

        uploadUniformFloat(name, value);
    }

    void Shader::setFloat2(const std::string& name, const glm::vec2& value)
    {
        OAK_PROFILE_FUNCTION();

        uploadUniformFloat2(name, value);
    }

    void Shader::setFloat3(const std::string& name, const glm::vec3& value)
    {
        OAK_PROFILE_FUNCTION();

        uploadUniformFloat3(name, value);
    }

    void Shader::setFloat4(const std::string& name, const glm::vec4& value)
    {
        OAK_PROFILE_FUNCTION();

        uploadUniformFloat4(name, value);
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& value)
    {
        OAK_PROFILE_FUNCTION();

        uploadUniformMat4(name, value);
    }

    void Shader::uploadUniformInt(const std::string& name, int value)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void Shader::uploadUniformFloat(const std::string& name, float value)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void Shader::uploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void Shader::uploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::uploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void Shader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        auto location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}
