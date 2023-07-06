#pragma once

#include "Oak/Core/Base.hpp"
#include "Oak/Core/Log.hpp"
#include <filesystem>

#ifdef OAK_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define OAK_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { OAK_LOG##type##ERROR(msg, __VA_ARGS__); OAK_DEBUGBREAK(); } }
#define OAK_INTERNAL_ASSERT_WITH_MSG(type, check, ...) OAK_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define OAK_INTERNAL_ASSERT_NO_MSG(type, check) OAK_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", OAK_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define OAK_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define OAK_INTERNAL_ASSERT_GET_MACRO(...) OAK_EXPAND_MACRO( OAK_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, OAK_INTERNAL_ASSERT_WITH_MSG, OAK_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define OAK_ASSERT(...) OAK_EXPAND_MACRO( OAK_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define OAK_CORE_ASSERT(...) OAK_EXPAND_MACRO( OAK_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define OAK_ASSERT(...)
#define OAK_CORE_ASSERT(...)
#endif
