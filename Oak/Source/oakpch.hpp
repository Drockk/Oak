#pragma once
#include "Oak/Core/PlatformDetection.hpp"

#ifdef OAK_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif // !NOMINMAX

#endif // OAK_PLATFORM_WINDOWS

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Oak/Core/Base.hpp"

#include "Oak/Core/Log.hpp"

#include "Oak/Debug/Instrumentor.hpp"

#ifdef OAK_PLATFORM_WINDOWS
#include <Windows.h>
#endif // OAK_PLATFORM_WINDOWS
