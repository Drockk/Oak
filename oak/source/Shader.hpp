#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

namespace oak
{
    class Shader
    {
    public:
        Shader() = default;
        ~Shader();

        void loadFromFile(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
        void loadFromString(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

        void use();

        void setInt(const std::string& name, int value);
        void setMat4(const std::string& name, glm::mat4 value);

    private:
        std::string readFile(const std::filesystem::path& path);
        GLuint createShader(GLenum type, const char* shaderSource);
        GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

        GLuint m_ProgramId{ 0 };
    };
}
