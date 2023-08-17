#pragma once
#include <span>

namespace oak {
    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    constexpr static uint32_t shaderDataTypeSize(ShaderDataType type)
    {
        switch (type) {
            case ShaderDataType::Float:
                return 4;
            case ShaderDataType::Float2:
                return 4 * 2;
            case ShaderDataType::Float3:
                return 4 * 3;
            case ShaderDataType::Float4:
                return 4 * 4;
            case ShaderDataType::Mat3:
                return 4 * 3 * 3;
            case ShaderDataType::Mat4:
                return 4 * 4 * 4;
            case ShaderDataType::Int:
                return 4;
            case ShaderDataType::Int2:
                return 4 * 2;
            case ShaderDataType::Int3:
                return 4 * 3;
            case ShaderDataType::Int4:
                return 4 * 4;
            case ShaderDataType::Bool:
                return 1;
        }

        OAK_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct BufferElement
    {
        std::string name{};
        ShaderDataType type{};
        uint32_t size{};
        size_t offset{};
        bool normalized{};

        BufferElement() = default;

        BufferElement(ShaderDataType t_type, const std::string& t_name, bool t_normalized = false): name(t_name), type(t_type), size(shaderDataTypeSize(t_type)), offset(0), normalized(t_normalized)
        {
        }

        constexpr uint32_t getComponentCount() const
        {
            switch (type)
            {
                case ShaderDataType::Float:
                    return 1;
                case ShaderDataType::Float2:
                    return 2;
                case ShaderDataType::Float3:
                    return 3;
                case ShaderDataType::Float4:
                    return 4;
                case ShaderDataType::Mat3:
                    return 3; // 3* float3
                case ShaderDataType::Mat4:
                    return 4; // 4* float4
                case ShaderDataType::Int:
                    return 1;
                case ShaderDataType::Int2:
                    return 2;
                case ShaderDataType::Int3:
                    return 3;
                case ShaderDataType::Int4:
                    return 4;
                case ShaderDataType::Bool:
                    return 1;
            }

            OAK_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() = default;

        BufferLayout(std::initializer_list<BufferElement> elements): m_Elements(elements)
        {
            calculateOffsetsAndStride();
        }

        uint32_t getStride() const { return m_Stride; }
        const std::vector<BufferElement>& getElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        constexpr void calculateOffsetsAndStride()
        {
            size_t offset{ 0 };
            m_Stride = 0;
            for (auto& element : m_Elements)
            {
                element.offset = offset;
                offset += element.size;
                m_Stride += element.size;
            }
        }

        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride{ 0 };
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual constexpr auto bind() -> void const = 0;
        virtual constexpr auto unbind() -> void const = 0;

        virtual constexpr auto setData(std::span<std::byte> t_indicies) -> void = 0;

        virtual constexpr auto getLayout() const -> const BufferLayout& = 0;
        virtual constexpr auto setLayout(const BufferLayout& t_layout) -> void = 0;

        static Ref<VertexBuffer> create(uint32_t size);
        static Ref<VertexBuffer> create(float* vertices, uint32_t size);
    };

    // Currently Oak only supports 32-bit index buffers
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual constexpr auto bind() -> void const = 0;
        virtual constexpr auto unbind() -> void const = 0;

        virtual constexpr auto getCount() -> uint32_t const = 0;

        static Ref<IndexBuffer> create(uint32_t* indices, uint32_t count);
    };
}
