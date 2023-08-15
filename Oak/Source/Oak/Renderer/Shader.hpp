#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

namespace oak {
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual void setInt(const std::string& name, int value) = 0;
        virtual void setIntArray(const std::string& name, int* values, uint32_t count) = 0;
        virtual void setFloat(const std::string& name, float value) = 0;
        virtual void setFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;

        virtual std::string_view getName() const = 0;

        static Ref<Shader> create(const std::string& filepath);
        static Ref<Shader> create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    };

    class ShaderLibrary
    {
    public:
        void add(const std::string& name, const Ref<Shader>& shader);
        void add(const Ref<Shader>& shader);
        Ref<Shader> load(const std::string& filepath);
        Ref<Shader> load(const std::string& name, const std::string& filepath);

        Ref<Shader> get(const std::string& name);

        bool exists(const std::string& name) const;

    private:
        std::unordered_map<std::string, oak::Ref<Shader>> m_Shaders;
    };
}
