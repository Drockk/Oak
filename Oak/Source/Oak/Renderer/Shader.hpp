#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace oak
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual void setInt(const std::string& t_name, int t_value) = 0;
        virtual void setIntArray(const std::string& t_name, int* t_values, uint32_t t_count) = 0;
        virtual void setFloat(const std::string& t_name, float t_value) = 0;
        virtual void setFloat2(const std::string& t_name, const glm::vec2& t_value) = 0;
        virtual void setFloat3(const std::string& t_name, const glm::vec3& t_value) = 0;
        virtual void setFloat4(const std::string& t_name, const glm::vec4& t_value) = 0;
        virtual void setMat4(const std::string& t_name, const glm::mat4& t_value) = 0;

        virtual const std::string& getName() const = 0;

        static Ref<Shader> create(const std::string& t_filepath);
        static Ref<Shader> create(const std::string& t_name, const std::string& t_vertexSrc, const std::string& t_fragmentSrc);
    };

    class ShaderLibrary
    {
    public:
        void add(const std::string& t_name, const Ref<Shader>& t_shader);
        void add(const Ref<Shader>& t_shader);
        Ref<Shader> load(const std::string& t_filepath);
        Ref<Shader> load(const std::string& t_name, const std::string& t_filepath);

        Ref<Shader> get(const std::string& t_name);

        bool exists(const std::string& t_name) const;

    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
}
