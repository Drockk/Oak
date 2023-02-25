#include "Application.hpp"

int main()
{
    oak::Application application("Oak!");

    application.init();
    application.run();
    application.shutdown();

    return 0;
}
