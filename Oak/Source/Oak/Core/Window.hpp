#pragma once
#include <string>

namespace oak
{
    class Window
    {
    public:
        Window() = default;
        virtual ~Window() = default;

        virtual void run() = 0;

        static std::unique_ptr<Window> create(const std::string& name, std::pair<uint32_t, uint32_t> resolution);
    };
}
