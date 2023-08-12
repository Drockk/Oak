#pragma once
#include "Oak/Core/Base.hpp"
#include "Oak/Core/Application.hpp"

#ifdef OAK_PLATFORM_WINDOWS

extern oak::Application* createApplication(oak::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    oak::Log::init();

    OAK_PROFILE_BEGIN_SESSION("Startup", "OakProfile-Startup.json");
    auto app = createApplication({ argc, argv });
    OAK_PROFILE_END_SESSION();

    OAK_PROFILE_BEGIN_SESSION("Runtime", "OakProfile-Runtime.json");
    app->run();
    OAK_PROFILE_END_SESSION();

    OAK_PROFILE_BEGIN_SESSION("Shutdown", "OakProfile-Shutdown.json");
    delete app;
    OAK_PROFILE_END_SESSION();
}

#endif
