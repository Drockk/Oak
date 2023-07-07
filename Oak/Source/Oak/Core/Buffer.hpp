#pragma once

#include <stdint.h>
#include <cstring>

namespace oak
{
    // Non-owning raw buffer class
    struct Buffer
    {
        uint8_t* data{ nullptr };
        size_t size{ 0 };

        Buffer() = default;
        Buffer(size_t t_size)
        {
            allocate(t_size);
        }
        Buffer(const Buffer&) = default;

        static Buffer copy(Buffer t_other)
        {
            Buffer result{ t_other.size };
            memcpy_s(result.data, result.size, t_other.data, t_other.size);
            return result;
        }

        void allocate(size_t t_size)
        {
            release();
            data = new uint8_t[t_size];
            size = t_size;
        }

        void release()
        {
            if (data)
            {
                delete[] data;
                data = nullptr;
                size = 0;
            }
        }

        template<typename T>
        T* as()
        {
            return reinterpret_cast<T*>(data);
        }

        operator bool() const
        {
            return static_cast<bool>(data);
        }
    };

    struct ScopedBuffer
    {
        ScopedBuffer(Buffer t_buffer): m_Buffer(t_buffer)
        {

        }

        ScopedBuffer(size_t t_size) : m_Buffer(t_size)
        {

        }

        ~ScopedBuffer()
        {
            m_Buffer.release();
        }

        uint8_t* data()
        {
            return m_Buffer.data;
        }

        size_t size()
        {
            return m_Buffer.size;
        }

        template<typename T>
        T* as()
        {
            return m_Buffer.as<T>();
        }

        operator bool() const
        {
            return m_Buffer;
        }

    private:
        Buffer m_Buffer{};
    };
}
