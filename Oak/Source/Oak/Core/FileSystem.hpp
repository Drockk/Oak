#pragma once

#include "Oak/Core/Buffer.hpp"

namespace oak
{
    class FileSystem
    {
    public:
        static Buffer readFileBinary(const std::filesystem::path& t_filepath);
    };
}
