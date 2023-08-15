#include "oakpch.hpp"
#include "Oak/Renderer/Shader.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Platform/OpenGL/Shader.hpp"

namespace oak {
    oak::Ref<Shader> Shader::create(const std::string& filepath)
    {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::Shader>(filepath);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None:
                OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return createRef<opengl::Shader>(name, vertexSrc, fragmentSrc);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader)
    {
        OAK_CORE_ASSERT(!exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::add(const Ref<Shader>& shader)
    {
        auto name = shader->getName();
        add(std::string(name), shader);
    }

    Ref<Shader> ShaderLibrary::load(const std::string& filepath)
    {
        auto shader = Shader::create(filepath);
        add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath)
    {
        auto shader = Shader::create(filepath);
        add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::get(const std::string& name)
    {
        OAK_CORE_ASSERT(exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    bool ShaderLibrary::exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
}
