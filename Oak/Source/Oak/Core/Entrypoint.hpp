#pragma once
#include "Oak/Core/Base.hpp"
#include "Oak/Core/Application.hpp"

//extern  std::unique_ptr<oak::Application> createApplication(oak::ApplicationCommandLineArgs t_args);

#ifdef OAK_PLATFORM_WINDOWS

int main(int argc, char** argv)
{
    try
    {
        oak::Log::init();

        OAK_PROFILE_BEGIN_SESSION("Startup", "OakProfile-Startup.json");
        auto app = createApplication({ argc, argv });
        OAK_PROFILE_END_SESSION();

        OAK_PROFILE_BEGIN_SESSION("Runtime", "OakProfile-Runtime.json");
        app->run();
        OAK_PROFILE_END_SESSION();

        OAK_PROFILE_BEGIN_SESSION("Shutdown", "OakProfile-Shutdown.json");
        app->shutdown();
        OAK_PROFILE_END_SESSION();
    }
    catch (const std::exception& t_exception)
    {
        OAK_LOG_CORE_CRITICAL(t_exception.what());
    }
}

#endif
