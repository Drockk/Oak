#pragma once

#include <string>

namespace oak
{
    class FileDialogs
    {
    public:
        static std::string openFile(const char* t_filter);
        static std::string saveFile(const char* t_filter);
    };

    class Time
    {
    public:
        static float getTime();
    };
}
