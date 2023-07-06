#pragma once

#include "Oak/Core/Base.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace oak {
    class Log
    {
    public:
        static void init();

        static Ref<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
        static Ref<spdlog::logger>& getClientLogger() { return s_ClientLogger; }
    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;
    };

}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
    return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
    return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}

// Core log macros
#define OAK_LOG_CORE_TRACE(...)    ::oak::Log::getCoreLogger()->trace(__VA_ARGS__)
#define OAK_LOG_CORE_INFO(...)     ::oak::Log::getCoreLogger()->info(__VA_ARGS__)
#define OAK_LOG_CORE_WARN(...)     ::oak::Log::getCoreLogger()->warn(__VA_ARGS__)
#define OAK_LOG_CORE_ERROR(...)    ::oak::Log::getCoreLogger()->error(__VA_ARGS__)
#define OAK_LOG_CORE_CRITICAL(...) ::oak::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define OAK_LOG_TRACE(...)         ::oak::Log::getClientLogger()->trace(__VA_ARGS__)
#define OAK_LOG_INFO(...)          ::oak::Log::getClientLogger()->info(__VA_ARGS__)
#define OAK_LOG_WARN(...)          ::oak::Log::getClientLogger()->warn(__VA_ARGS__)
#define OAK_LOG_ERROR(...)         ::oak::Log::getClientLogger()->error(__VA_ARGS__)
#define OAK_LOG_CRITICAL(...)      ::oak::Log::getClientLogger()->critical(__VA_ARGS__)
