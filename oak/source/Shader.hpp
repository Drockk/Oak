#pragma once
#include <glad/gl.h>

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

    private:
        std::string readFile(const std::filesystem::path& path);
        GLuint createShader(GLenum type, const char* shaderSource);
        GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

        GLuint m_ProgramId{ 0 };
    };
}
