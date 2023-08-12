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
        static GLenum shaderTypeFromString(const std::string& type)
        {
            if (type == "vertex") {
                return GL_VERTEX_SHADER;
            }
            if (type == "fragment" || type == "pixel") {
                return GL_FRAGMENT_SHADER;
            }

            OAK_CORE_ASSERT(false, "Unknown shader type!");
            return 0;
        }

        static shaderc_shader_kind glShaderStageToShaderC(GLenum stage)
        {
            switch (stage){
            case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
            case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
            }

            OAK_CORE_ASSERT(false);
            return (shaderc_shader_kind)0;
        }

        static const char* glShaderStageToString(GLenum stage)
        {
            switch (stage) {
            case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
            case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
            }

            OAK_CORE_ASSERT(false);
            return nullptr;
        }

        static const char* getCacheDirectory()
        {
            // TODO: make sure the assets directory is valid
            return "assets/cache/shader/opengl";
        }

        static void createCacheDirectoryIfNeeded()
        {
            std::string cacheDirectory = getCacheDirectory();
            if (!std::filesystem::exists(cacheDirectory)) {
                std::filesystem::create_directories(cacheDirectory);
            }
        }

        static const char* glShaderStageCachedOpenGLFileExtension(uint32_t stage)
        {
            switch (stage) {
                case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
                case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
            }

            OAK_CORE_ASSERT(false);
            return "";
        }

        static const char* glShaderStageCachedVulkanFileExtension(uint32_t stage)
        {
            switch (stage) {
            case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
            case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
            }

            OAK_CORE_ASSERT(false);
            return "";
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
            OAK_LOG_CORE_WARN("Shader creation took {0} ms", timer.elapsedMillis());
        }

        // Extract name from filepath
        auto lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = filepath.rfind('.');
        auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filepath.substr(lastSlash, count);
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

    std::string Shader::readFile(const std::string& filepath)
    {
        OAK_PROFILE_FUNCTION();

        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
        if (in) {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            if (size != -1) {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
            else {
                OAK_LOG_CORE_ERROR("Could not read from file '{0}'", filepath);
            }
        }
        else {
            OAK_LOG_CORE_ERROR("Could not open file '{0}'", filepath);
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
        GLuint program = glCreateProgram();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        const auto optimize = true;
        if (optimize) {
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
        const auto optimize = false;
        if (optimize) {
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
        GLuint program = glCreateProgram();

        std::vector<GLuint> shaderIDs;
        for (auto&& [stage, spirv] : m_OpenGLSPIRV) {
            GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
            glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
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

            for (auto id : shaderIDs)
                glDeleteShader(id);
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
            uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            int memberCount = bufferType.member_types.size();

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
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void Shader::uploadUniformFloat(const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void Shader::uploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void Shader::uploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::uploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void Shader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

}
