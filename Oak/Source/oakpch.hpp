#pragma once

#include "Oak/Core/PlatformDetection.hpp"

#ifdef OAK_PLATFORM_WINDOWS
    #ifndef NOMINMAX
        // See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
        #define NOMINMAX
    #endif
#endif

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Oak/Core/Base.hpp"

#include "Oak/Core/Log.hpp"

#include "Oak/Debug/Instrumentor.hpp"

#ifdef OAK_PLATFORM_WINDOWS
    #include <Windows.h>
#endif
