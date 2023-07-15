#pragma once

namespace oak
{
    class UniformBuffer
    {
    public:
        virtual ~UniformBuffer() = default;
        virtual void setData(const void* t_data, uint32_t t_size, uint32_t t_offset = 0) = 0;

        static Ref<UniformBuffer> create(uint32_t t_size, uint32_t t_binding);
    };
}
