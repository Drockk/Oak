#include "oakpch.hpp"
#include "Oak/Renderer/Shader.hpp"

#include "Oak/Renderer/Renderer.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

namespace oak
{
    Ref<Shader> Shader::create(const std::string& t_filepath)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<openGL::Shader>(t_filepath);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::create(const std::string& t_name, const std::string& t_vertexSrc, const std::string& t_fragmentSrc)
    {
        switch (Renderer::getAPI())
        {
        case RendererAPI::API::None:
            OAK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<openGL::Shader>(t_name, t_vertexSrc, t_fragmentSrc);
        }

        OAK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    void ShaderLibrary::add(const std::string& t_name, const Ref<Shader>& t_shader)
    {
        OAK_CORE_ASSERT(!exists(t_name), "Shader already exists!");
        m_Shaders[t_name] = t_shader;
    }

    void ShaderLibrary::add(const Ref<Shader>& t_shader)
    {
        auto& name = t_shader->getName();
        add(name, t_shader);
    }

    Ref<Shader> ShaderLibrary::load(const std::string& t_filepath)
    {
        auto shader = Shader::create(t_filepath);
        add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::load(const std::string& t_name, const std::string& t_filepath)
    {
        auto shader = Shader::create(t_filepath);
        add(t_name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::get(const std::string& t_name)
    {
        OAK_CORE_ASSERT(exists(t_name), "Shader not found!");
        return m_Shaders[t_name];
    }

    bool ShaderLibrary::exists(const std::string& t_name) const
    {
        return m_Shaders.find(t_name) != m_Shaders.end();
    }
}
