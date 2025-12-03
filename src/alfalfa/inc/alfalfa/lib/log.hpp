#pragma once

#include <sstream>
#include <ostream>
#include <string>
#include <cstdint>
#include <utility>
#include <concepts>

#include <alfalfa/lib/errors.hpp>


namespace alfalfa
{
namespace logging
{
    enum class LogLevel : uint8_t
    {
        None = 0,
        Error = 1,
        Warning = 2,
        Info = 3,
        Debug = 4,
        Verbose = 5
    };

    namespace internal
    {
        extern bool enabled;
    }

    const std::string& ToString(const LogLevel level);
    const std::string& ToLogString(const LogLevel level);

    alfalfa::ErrorCode Init();
    alfalfa::ErrorCode SetLevel(const std::string& value);
    alfalfa::ErrorCode SetLevel(const LogLevel value);

    std::ostream& GetStdout();
    void SetStdout(std::shared_ptr<std::ostream> output_device);

    namespace implementation
    {
        void SetLevel(const LogLevel value);
        uint32_t GetTimestamp();

        void LogImpl(int64_t message);
        void LogImpl(uint64_t message);

        void LogImpl(int32_t message);
        void LogImpl(uint32_t message);

        void LogImpl(int message);
        void LogImpl(unsigned int message);

        // Covered by unsigned int
        // void LogImpl(std::size_t message);

        void LogImpl(int16_t message);
        void LogImpl(uint16_t message);

        void LogImpl(int8_t message);
        void LogImpl(uint8_t message);

        void LogImpl(bool message);

        void LogImpl(const char* message);

        template<std::size_t SIZE>
        void LogImplArray(const char (&message)[SIZE])
        {
            LogImpl(message);
        }

        inline void LogImpl(char* message)
        {
            LogImpl((const char*) message);
        }

        inline void LogImpl(const unsigned char* message)
        {
            LogImpl((const char*) message);
        }

        inline void LogImpl(unsigned char* message)
        {
            LogImpl((const char*) message);
        }

        template<std::size_t SIZE>
        inline void LogImpl(const char (&message)[SIZE])
        {
            LogImpl(&message[0]);
        }

        void LogImpl(const std::string& message);

        void LogImpl(const std::string&& message);

        void LogImpl(std::stringstream& message);

        void LogImpl(std::stringstream&& message);

        void LogImpl(const std::string_view& message);

        void LogImpl(const std::string_view&& message);

        template<typename T>
        void LogImpl(const T message) requires std::is_enum_v<T>
        {
            LogImpl(static_cast<std::underlying_type_t<T>>(message));
        }

        void LogImplLn();

    }

    alfalfa::ErrorCode SetLevel(const LogLevel value);
    LogLevel GetCurrentLevel();
    std::optional<LogLevel> GetLevel(const std::string& value);

    class Logger
    {
        public:
            Logger(const char* header) : _header(header)
            {
            }

            template<typename... Args>
            inline void Trace(Args&&... args) const
            {
                Write(LogLevel::Verbose, std::forward<Args>(args)...);
            }

            template<typename... Args>
            inline void Debug(Args&&... args) const
            {
                Write(LogLevel::Debug, std::forward<Args>(args)...);
            }

            template<typename... Args>
            inline void Info(Args&&... args) const
            {
                Write(LogLevel::Info, std::forward<Args>(args)...);
            }

            template<typename... Args>
            inline void Warning(Args&&... args) const
            {
                Write(LogLevel::Warning, std::forward<Args>(args)...);
            }

            template<typename... Args>
            inline void Error(Args&&... args) const
            {
                Write(LogLevel::Error, std::forward<Args>(args)...);
            }

        protected:
            const char* _header;

            template<typename... Args>
            inline void Write(LogLevel level, Args&&... args) const;
    };

    class LoggerNull
    {
        public:
            LoggerNull(const char* header)
            {
                (void) header;
            }

            template<typename... Args>
            inline void Trace(Args&&... args) const
            {
            }

            template<typename... Args>
            inline void Debug(Args&&... args) const
            {
            }

            template<typename... Args>
            inline void Info(Args&&... args) const
            {
            }

            template<typename... Args>
            inline void Warning(Args&&... args) const
            {
            }

            template<typename... Args>
            inline void Error(Args&&... args) const
            {
            }
    };

}
}


inline void LogDirect()
{
}

template<typename T, typename... Args>
inline void LogDirect(T&& value, Args&&... args)
{
    alfalfa::logging::implementation::LogImpl(std::forward<T>(value));
    LogDirect(std::forward<Args>(args)...);
}

template<typename... Args>
inline void LoglnDirect(Args&&... args)
{
    LogDirect(std::forward<Args>(args)...);
    alfalfa::logging::implementation::LogImplLn();
}


inline void Log()
{
}

template<typename... Args>
inline void Log(Args&&... args)
{
    if (alfalfa::logging::internal::enabled)
    {
        LogDirect(std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void Logln(Args&&... args)
{
    if (alfalfa::logging::internal::enabled)
    {
        LoglnDirect(std::forward<Args>(args)...);
    }
}


namespace alfalfa
{
namespace logging
{

    template<typename... Args>
    inline void Logger::Write(LogLevel level, Args&&... args) const
    {
        if (level > GetCurrentLevel() or not internal::enabled)
        {
            return;
        }
        const auto ts = alfalfa::logging::implementation::GetTimestamp();
        Log(ts, " [", ToLogString(level), "] ", _header, ": ", std::forward<Args>(args)...);
        // Log(std::forward<Args>(args)...);
        alfalfa::logging::implementation::LogImplLn();
    }

}
}
