#pragma once

namespace oak
{
    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static uint32_t shaderDataTypeSize(ShaderDataType t_type)
    {
        switch (t_type)
        {
        case oak::ShaderDataType::None:
            break;
        case oak::ShaderDataType::Float:
            return 4;
        case oak::ShaderDataType::Float2:
            return 4 * 2;
        case oak::ShaderDataType::Float3:
            return 4 * 3;
        case oak::ShaderDataType::Float4:
            return 4 * 4;
        case oak::ShaderDataType::Mat3:
            return 4 * 3 * 3;
        case oak::ShaderDataType::Mat4:
            return 4 * 4 * 4;
        case oak::ShaderDataType::Int:
            return 4;
        case oak::ShaderDataType::Int2:
            return 4 * 2;
        case oak::ShaderDataType::Int3:
            return 4 * 3;
        case oak::ShaderDataType::Int4:
            return 4 * 4;
        case oak::ShaderDataType::Bool:
            return 1;
        default:
            break;
        }

        OAK_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return {};
    }

    struct BufferElement
    {
        std::string name{};
        ShaderDataType type{};
        uint32_t size{};
        size_t offset{};
        bool normalized{};

        BufferElement() = default;
        BufferElement(ShaderDataType t_type, const std::string& t_name, bool t_normalized = false)
            : name{ t_name }, type{ t_type }, size{ shaderDataTypeSize(t_type) }, offset{ 0 }, normalized{ t_normalized }
        {

        }

        uint32_t getComponentCount() const
        {
            switch (type)
            {
            case oak::ShaderDataType::None:
                break;
            case oak::ShaderDataType::Float:
                return 1;
            case oak::ShaderDataType::Float2:
                return 2;
            case oak::ShaderDataType::Float3:
                return 3;
            case oak::ShaderDataType::Float4:
                return 4;
            case oak::ShaderDataType::Mat3:
                return 3;
            case oak::ShaderDataType::Mat4:
                return 4;
            case oak::ShaderDataType::Int:
                return 1;
            case oak::ShaderDataType::Int2:
                return 2;
            case oak::ShaderDataType::Int3:
                return 3;
            case oak::ShaderDataType::Int4:
                return 4;
            case oak::ShaderDataType::Bool:
                return 1;
            default:
                break;
            }

            OAK_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return {};
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() = default;
        BufferLayout(std::initializer_list<BufferElement> t_elements): m_Elements{ t_elements }
        {
            calculateOffsetsAndStride();
        }
        ~BufferLayout() = default;

        uint32_t getStride() const { return m_Stride; }
        const std::vector<BufferElement>& getElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        void calculateOffsetsAndStride()
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

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual void setData(const void* t_data, uint32_t t_size) = 0;

        virtual const BufferLayout& getLayout() const = 0;
        virtual void setLayout(const BufferLayout& t_layout) = 0;

        static Ref<VertexBuffer> create(uint32_t size);
        static Ref<VertexBuffer> create(float* t_vertices, uint32_t t_size);
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual uint32_t getCount() const = 0;

        static Ref<IndexBuffer> create(uint32_t* t_indices, uint32_t t_count);
    };
}
