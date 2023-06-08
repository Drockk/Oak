#pragma once
// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace oak
{
    class Log
    {
    public:
        Log() = default;
        ~Log() = default;

        static void init();

        static std::shared_ptr<spdlog::logger>& getCoreLogger()
        {
            return s_CoreLogger;
        }
        static std::shared_ptr<spdlog::logger>& getClientLogger()
        {
            return s_ClientLogger;
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
} // namespace oak

// Core log macros
#define OAK_CORE_TRACE(...)    ::oak::Log::getCoreLogger()->trace(__VA_ARGS__)
#define OAK_CORE_DEBUG(...)    ::oak::Log::getCoreLogger()->debug(__VA_ARGS__)
#define OAK_CORE_INFO(...)     ::oak::Log::getCoreLogger()->info(__VA_ARGS__)
#define OAK_CORE_WARN(...)     ::oak::Log::getCoreLogger()->warn(__VA_ARGS__)
#define OAK_CORE_ERROR(...)    ::oak::Log::getCoreLogger()->error(__VA_ARGS__)
#define OAK_CORE_CRITICAL(...) ::oak::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define OAK_TRACE(...)         ::oak::Log::getClientLogger()->trace(__VA_ARGS__)
#define OAK_DEBUG(...)         ::oak::Log::getClientLogger()->debug(__VA_ARGS__)
#define OAK_INFO(...)          ::oak::Log::getClientLogger()->info(__VA_ARGS__)
#define OAK_WARN(...)          ::oak::Log::getClientLogger()->warn(__VA_ARGS__)
#define OAK_ERROR(...)         ::oak::Log::getClientLogger()->error(__VA_ARGS__)
#define OAK_CRITICAL(...)      ::oak::Log::getClientLogger()->critical(__VA_ARGS__)

