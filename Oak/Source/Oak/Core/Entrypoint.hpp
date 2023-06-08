#pragma once
#include "Application.hpp"

int main()
{
    auto app = createApplication();

    app->init();
    app->run();
    app->shutdown();

    return 0;
}
