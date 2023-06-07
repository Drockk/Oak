#include "Core/Application.hpp"
#include "Core/Log.hpp"

namespace oak
{
    void Application::init()
    {
        oak::Log::init();
        OAK_CORE_INFO("I am alive!");
    }

    void Application::run()
    {

    }

    void Application::shutdown()
    {

    }
}
