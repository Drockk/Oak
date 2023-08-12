#pragma once

#include "Oak/Core/Buffer.hpp"

namespace oak {
    class FileSystem
    {
    public:
        // TODO: move to FileSystem class
        static Buffer readFileBinary(const std::filesystem::path& filepath);
    };
}
