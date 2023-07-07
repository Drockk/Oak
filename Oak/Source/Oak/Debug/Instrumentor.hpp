#pragma once

#include "Oak/Core/Log.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

namespace oak
{
    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult
    {
        std::string name{};

        FloatingPointMicroseconds start{};
        std::chrono::microseconds elapsedTime{};
        std::thread::id threadID{};
    };

    struct InstrumentationSession
    {
        std::string name{};
    };

    class Instrumentor
    {
    public:
        Instrumentor(const Instrumentor&) = delete;
        Instrumentor(Instrumentor&&) = delete;

        void beginSession(const std::string& t_name, const std::string& t_filepath = "results.json")
        {
            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession)
            {
                // If there is already a current session, then close it before beginning new one.
                // Subsequent profiling output meant for the original session will end up in the
                // newly opened session instead.  That's better than having badly formatted
                // profiling output.
                if (Log::getCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
                {
                    OAK_LOG_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", t_name, m_CurrentSession->name);
                }
                internalEndSession();
            }
            m_OutputStream.open(t_filepath);

            if (m_OutputStream.is_open())
            {
                m_CurrentSession = new InstrumentationSession({ t_name });
                writeHeader();
            }
            else
            {
                if (Log::getCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
                {
                    OAK_LOG_CORE_ERROR("Instrumentor could not open results file '{0}'.", t_filepath);
                }
            }
        }

        void endSession()
        {
            std::lock_guard lock(m_Mutex);
            internalEndSession();
        }

        void writeProfile(const ProfileResult& t_result)
        {
            std::stringstream json;

            json << std::setprecision(3) << std::fixed;
            json << ",{";
            json << "\"cat\":\"function\",";
            json << "\"dur\":" << (t_result.elapsedTime.count()) << ',';
            json << "\"name\":\"" << t_result.name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << t_result.threadID << ",";
            json << "\"ts\":" << t_result.start.count();
            json << "}";

            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession)
            {
                m_OutputStream << json.str();
                m_OutputStream.flush();
            }
        }

        static Instrumentor& get()
        {
            static Instrumentor instance;
            return instance;
        }
    private:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
        }

        ~Instrumentor()
        {
            endSession();
        }

        void writeHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();
        }

        void writeFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        // Note: you must already own lock on m_Mutex before
        // calling InternalEndSession()
        void internalEndSession()
        {
            if (m_CurrentSession)
            {
                writeFooter();
                m_OutputStream.close();
                delete m_CurrentSession;
                m_CurrentSession = nullptr;
            }
        }
    private:
        std::mutex m_Mutex;
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* t_name)
            : m_Name(t_name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::steady_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                stop();
        }

        void stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();
            auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
            auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

            Instrumentor::get().writeProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

            m_Stopped = true;
        }
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
        bool m_Stopped;
    };

    namespace InstrumentorUtils {

        template <size_t N>
        struct ChangeResult
        {
            char data[N];
        };

        template <size_t N, size_t K>
        constexpr auto cleanupOutputString(const char(&t_expr)[N], const char(&t_remove)[K])
        {
            ChangeResult<N> result = {};

            size_t srcIndex = 0;
            size_t dstIndex = 0;
            while (srcIndex < N)
            {
                size_t matchIndex = 0;
                while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && t_expr[srcIndex + matchIndex] == t_remove[matchIndex])
                    matchIndex++;
                if (matchIndex == K - 1)
                    srcIndex += matchIndex;
                result.data[dstIndex++] = t_expr[srcIndex] == '"' ? '\'' : t_expr[srcIndex];
                srcIndex++;
            }
            return result;
        }
    }
}

#define OAK_PROFILE 1
#if OAK_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define OAK_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define OAK_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define OAK_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define OAK_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define OAK_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define OAK_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define OAK_FUNC_SIG __func__
#else
#define OAK_FUNC_SIG "OAK_FUNC_SIG unknown!"
#endif

#define OAK_PROFILE_BEGIN_SESSION(name, filepath) ::oak::Instrumentor::get().beginSession(name, filepath)
#define OAK_PROFILE_END_SESSION() ::oak::Instrumentor::get().endSession()
#define OAK_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::oak::InstrumentorUtils::cleanupOutputString(name, "__cdecl ");\
                                               ::oak::InstrumentationTimer timer##line(fixedName##line.data)
#define OAK_PROFILE_SCOPE_LINE(name, line) OAK_PROFILE_SCOPE_LINE2(name, line)
#define OAK_PROFILE_SCOPE(name) OAK_PROFILE_SCOPE_LINE(name, __LINE__)
#define OAK_PROFILE_FUNCTION() OAK_PROFILE_SCOPE(OAK_FUNC_SIG)
#else
#define OAK_PROFILE_BEGIN_SESSION(name, filepath)
#define OAK_PROFILE_END_SESSION()
#define OAK_PROFILE_SCOPE(name)
#define OAK_PROFILE_FUNCTION()
#endif
