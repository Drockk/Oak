#pragma once
#include <any>
#include <memory>

namespace oak
{
    class GraphicsContext
    {
    public:
        GraphicsContext() = default;
        ~GraphicsContext() = default;

        virtual void init() = 0;
        virtual void swapBuffers() = 0;

        static std::shared_ptr<GraphicsContext> create(std::any t_window);
    };
}
