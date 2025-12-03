#include <sstream>
#include <string>
#include <optional>

#include <alfalfa/lib/log.hpp>
#include <alfalfa/lib/types/null_stream.hpp>


namespace alfalfa
{
namespace logging
{
    auto null_stream = std::make_shared<alfalfa::lib::NullStream>();

    namespace internal
    {
        alfalfa::logging::LogLevel current_level = alfalfa::logging::LogLevel::None;
        bool enabled = false;
        std::shared_ptr<std::ostream> output_device = null_stream;
    }

    const auto _level_none = std::string("none");
    const auto _level_error =   std::string("error");
    const auto _level_warning = std::string("warning");
    const auto _level_info =    std::string("info");
    const auto _level_debug =   std::string("debug");
    const auto _level_verbose = std::string("verbose");

    const auto _level_log_none =    std::string("   none");
    const auto _level_log_error =   std::string("  error");
    const auto _level_log_warning = std::string("warning");
    const auto _level_log_info =    std::string("   info");
    const auto _level_log_debug =   std::string("  debug");
    const auto _level_log_verbose = std::string("verbose");

    std::optional<LogLevel> GetLevel(const std::string& value)
    {
        if (value == _level_none)
        {
            return LogLevel::None;
        }
        else if (value == _level_error)
        {
            return LogLevel::Error;
        }
        else if (value == _level_warning)
        {
            return LogLevel::Warning;
        }
        else if (value == _level_info)
        {
            return LogLevel::Info;
        }
        else if (value == _level_debug)
        {
            return LogLevel::Debug;
        }
        else if (value == _level_verbose)
        {
            return LogLevel::Verbose;
        }
        return std::nullopt;
    }

    LogLevel GetCurrentLevel()
    {
        return internal::current_level;
    }
        alfalfa::ErrorCode SetLevel(const LogLevel value)
        {
            implementation::SetLevel(value);
            internal::enabled = value != LogLevel::None;
            internal::current_level = value;
            return alfalfa::ErrorCode::None;
        }

    const std::string& ToString(const LogLevel level)
    {
        switch (level)
        {
            case LogLevel::None:        return _level_none;
            case LogLevel::Error:       return _level_error;
            case LogLevel::Warning:     return _level_warning;
            case LogLevel::Info:        return _level_info;
            case LogLevel::Debug:       return _level_debug;
            case LogLevel::Verbose:     return _level_verbose;
            default:                    return _level_none;
        }
    }

    const std::string& ToLogString(const LogLevel level)
    {
        switch (level)
        {
            case LogLevel::None:        return _level_log_none;
            case LogLevel::Error:       return _level_log_error;
            case LogLevel::Warning:     return _level_log_warning;
            case LogLevel::Info:        return _level_log_info;
            case LogLevel::Debug:       return _level_log_debug;
            case LogLevel::Verbose:     return _level_log_verbose;
            default:                    return _level_none;
        }
    }

    alfalfa::ErrorCode SetLevel(const std::string& value)
    {
        auto level = GetLevel(value);

        if (level.has_value())
        {
            return SetLevel(level.value());
        }
        return alfalfa::ErrorCode::InvalidValue;
    }

    std::ostream& GetStdout()
    {
        return *internal::output_device;
    }

    void SetStdout(std::shared_ptr<std::ostream> output_device)
    {
        if (output_device)
        {
            internal::output_device = output_device;
        }
        else
        {
            internal::output_device = null_stream;
        }
    }

    namespace implementation
    {
        /* Implement the functions from alfalfa/lib/log.hpp */
        void LogImpl(int64_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(uint64_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(int32_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(uint32_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(int message)
        {
            GetStdout() << message;
        }

        void LogImpl(unsigned int message)
        {
            GetStdout() << message;
        }

        void LogImpl(int16_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(uint16_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(int8_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(uint8_t message)
        {
            GetStdout() << message;
        }

        void LogImpl(bool message)
        {
            GetStdout() << message;
        }

        void LogImpl(const char* message)
        {
            GetStdout() << message;
        }

        void LogImpl(const std::string& message)
        {
            GetStdout() << message;
        }

        void LogImpl(const std::string&& message)
        {
            GetStdout() << message;
        }

        void LogImpl(std::stringstream& message)
        {
            if (not internal::output_device)
            {
                return;
            }
            const auto csize = 256;
            std::array<char, csize> temp;
            while(true)
            {
                temp.fill(0x00);
                message.get(temp.begin(), csize - 1);
                auto rchars = message.gcount();
                if (not rchars)
                {
                    break;
                }
                *internal::output_device << std::string_view(temp.data(), rchars);
            }
        }

        void LogImpl(std::stringstream&& message)
        {
            if (not internal::output_device)
            {
                return;
            }
            const auto csize = 256;
            std::array<char, csize> temp;
            while(true)
            {
                temp.fill(0x00);
                message.get(temp.begin(), csize - 1);
                auto rchars = message.gcount();
                if (not rchars)
                {
                    break;
                }
                *internal::output_device << std::string_view(temp.data(), rchars);
            }
        }

        void LogImpl(const std::string_view& message)
        {
            if (not internal::output_device)
            {
                return;
            }
            for(auto it = message.begin(); it != message.end(); ++it)
            {
                *internal::output_device << it;
            }
        }

        void LogImpl(const std::string_view&& message)
        {
            if (not internal::output_device)
            {
                return;
            }
            for(auto it = message.begin(); it != message.end(); ++it)
            {
                *internal::output_device << it;
            }
        }

        void LogImplLn()
        {
            // Provide empty function for parameter pack expansion
            GetStdout() << '\n';
        }
    }

}
}
