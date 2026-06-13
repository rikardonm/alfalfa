#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <optional>
#include <exception>

#include <nlohmann/json.hpp>


namespace alfalfa
{
    /** @brief Global operation return codes. */
    enum class ErrorCode : uint32_t
    {
        /// @brief No error.
        None                    = 0x0000'0000,
        /// @brief Undefined error type, catch-all.
        UnknownError            = 0x0000'0001,
        /// @brief Element not found.
        NotFound                = 0x0000'0002,
        /// @brief Length of value or argument exceeds expectations.
        TooLong                 = 0x0000'0003,
        /// @brief Value or argument falls beyond supported range.
        OutOfBounds             = 0x0000'0004,
        /// @brief Value or argument is invalid.
        InvalidValue            = 0x0000'0005,
        /// @brief Operation forbidden for used access level.
        InsufficientAccess      = 0x0000'0006,
        /// @brief Operation unable to be completed with current system state.
        InvalidState            = 0x0000'0007,
        /// @brief The supplied value does not fit into the expected options.
        InvalidChoice           = 0x0000'0008,
        /// @brief The operation is asynchronous and will complete independently.
        AsyncOperationStarted   = 0x0000'0009,
        /// @brief The operation failed due to insufficient memory.
        OutOfMemory             = 0x0000'000A,
        /// @brief The operation failed due to a lower level layer error.
        LowerLevelError         = 0x0000'000B,
        /// @brief The operation failed due to a timeout.
        Timeout                 = 0x0000'000C,
    };

    inline const char* ToCString(const ErrorCode code)
    {
        using EC = ErrorCode;
        switch(code)
        {
            case EC::None:                  return "None";
            case EC::UnknownError:          return "UnknownError";
            case EC::NotFound:              return "NotFound";
            case EC::TooLong:               return "TooLong";
            case EC::OutOfBounds:           return "OutOfBounds";
            case EC::InvalidValue:          return "InvalidValue";
            case EC::InsufficientAccess:    return "InsufficientAccess";
            case EC::InvalidState:          return "InvalidState";
            case EC::InvalidChoice:         return "InvalidChoice";
            case EC::AsyncOperationStarted: return "AsyncOperationStarted";
            case EC::OutOfMemory:           return "OutOfMemory";
            case EC::LowerLevelError:       return "LowerLevelError";
            case EC::Timeout:               return "Timeout";
            default:                        return "?";
        }
    }

    inline void to_json(nlohmann::json& j, const ErrorCode& code)
    {
        j = ToCString(code);
    }

    inline void from_json(const nlohmann::json& j, ErrorCode& code)
    {
        std::string str = j.get<std::string>();
        if (str == "None")                  code = ErrorCode::None;
        else if (str == "UnknownError")     code = ErrorCode::UnknownError;
        else if (str == "NotFound")         code = ErrorCode::NotFound;
        else if (str == "TooLong")          code = ErrorCode::TooLong;
        else if (str == "OutOfBounds")      code = ErrorCode::OutOfBounds;
        else if (str == "InvalidValue")     code = ErrorCode::InvalidValue;
        else if (str == "InsufficientAccess") code = ErrorCode::InsufficientAccess;
        else if (str == "InvalidState")     code = ErrorCode::InvalidState;
        else if (str == "InvalidChoice")    code = ErrorCode::InvalidChoice;
        else if (str == "AsyncOperationStarted") code = ErrorCode::AsyncOperationStarted;
        else if (str == "OutOfMemory")      code = ErrorCode::OutOfMemory;
        else if (str == "LowerLevelError")  code = ErrorCode::LowerLevelError;
        else if (str == "Timeout")          code = ErrorCode::Timeout;
        else
        {
            code = ErrorCode::UnknownError;
        }
    }

    inline std::string ToString(const ErrorCode code)
    {
        return ToCString(code);
    }

    class Exception : public std::exception
    {
    public:
        Exception(const ErrorCode code, const char* message = nullptr)
            : std::exception()
            , _code(code)
            , _message(message ? message : ToCString(code))
        {}

        const char* what() const noexcept override
        {
            return _message;
        }

        ErrorCode Code() const
        {
            return _code;
        }

    private:
        ErrorCode _code;
        const char* _message;
    };

}
