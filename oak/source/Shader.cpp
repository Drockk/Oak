#include "Shader.hpp"

#include <fstream>
#include <iostream>

namespace oak
{
    Shader::~Shader()
    {
        glDeleteProgram(m_ProgramId);
    }

    void Shader::loadFromFile(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
    {
        auto vertexShaderSource = readFile(vertexShaderFile);
        auto fragmentShaderSource = readFile(fragmentShaderFile);

        loadFromString(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
    }

    void Shader::loadFromString(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
    {
        //Vertex Shader
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());

        //Fragment Shader
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

        //Create program
        m_ProgramId = createProgram(vertexShader, fragmentShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Shader::use()
    {
        glUseProgram(m_ProgramId);
    }

    void Shader::setInt(const std::string& name, int value)
    {
        glUniform1i(glGetUniformLocation(m_ProgramId, name.c_str()), value);
    }

    std::string Shader::readFile(const std::filesystem::path& path)
    {
        if (!std::filesystem::is_regular_file(path))
        {
            return {};
        }

        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            return { };
        }

        std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

        file.close();

        return content;
    }

    GLuint Shader::createShader(GLenum type, const char* shaderSource)
    {
        auto shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderSource, nullptr);
        glCompileShader(shader);

        auto success = 0;
        char infoLog[512] = { 0 };
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        return shader;
    }

    GLuint Shader::createProgram(GLuint vertexShader, GLuint fragmentShader)
    {
        auto programId = glCreateProgram();
        glAttachShader(programId, vertexShader);
        glAttachShader(programId, fragmentShader);
        glLinkProgram(programId);

        auto success = 0;
        char infoLog[512] = { 0 };
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        return programId;
    }
}
