#pragma once
#include <memory>

namespace oak
{
    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        void init();
        void run();
        void shutdown();

    private:
    };
}

std::unique_ptr<oak::Application> createApplication();
