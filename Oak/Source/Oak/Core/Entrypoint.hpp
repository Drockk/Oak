#pragma once
#include "Oak/Core/Application.hpp"

#include <iostream>

int main()
{
    try {
        auto app = createApplication();

        app->init();
        app->run();
        app->shutdown();
    }
    catch (std::exception& e) {
        std::cerr << e.what();
    }
    return 0;
}
