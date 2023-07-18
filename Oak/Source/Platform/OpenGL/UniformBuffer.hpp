#pragma once

#include "Oak/Renderer/UniformBuffer.hpp"

namespace openGL
{
    class UniformBuffer : public oak::UniformBuffer
    {
    public:
        UniformBuffer(uint32_t size, uint32_t binding);
        ~UniformBuffer() override;

        void setData(const void* data, uint32_t size, uint32_t offset = 0) override;

    private:
        uint32_t m_RendererID = 0;
    };
}
