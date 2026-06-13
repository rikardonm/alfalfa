#include <sstream>
#include <string>
#include <optional>

#include <alfalfa/lib/log.hpp>

#include "esp_log.h"


namespace alfalfa
{
namespace logging
{
    // ArduinoNvs _nvs;
    const auto _nvs_key = std::string("level");

    std::optional<esp_log_level_t> GetEspLevel(const std::optional<LogLevel> level)
    {
        if (level.has_value())
        {
            return GetEspLevel(level.value());
        }
        return std::nullopt;
    }

    std::optional<esp_log_level_t> GetEspLevel(const LogLevel level)
    {
        switch (level)
        {
            case LogLevel::None:        return ESP_LOG_NONE;
            case LogLevel::Error:       return ESP_LOG_ERROR;
            case LogLevel::Warning:     return ESP_LOG_WARN;
            case LogLevel::Info:        return ESP_LOG_INFO;
            case LogLevel::Debug:       return ESP_LOG_DEBUG;
            case LogLevel::Verbose:     return ESP_LOG_VERBOSE;
            default:                    return std::nullopt;
        }
    }

    alfalfa::ErrorCode Init()
    {
        // auto success = _nvs.begin("logging");
        // well... if it fails, what do we do, after all?
        // if (success)
        // {
        //     std::string n_level;
        //     if (_nvs.getString(_nvs_key, n_level))
        //     {
        //         return SetLevel(n_level);
        //     }
        //     else
        //     {
        //         _nvs.setString(_nvs_key, ToString(LogLevel::None));
        //     }
        //     return alfalfa::ErrorCode::None;
        // }
        // return alfalfa::ErrorCode::UnknownError;
        return alfalfa::ErrorCode::None;
    }

    namespace implementation
    {

        void SetLevel(const LogLevel value)
        {
            auto level = GetEspLevel(value);
            if (level.has_value())
            {
                esp_log_level_set("*", level.value());
            }
            // _nvs.setString(_nvs_key, ToString(value));
        }

        uint32_t GetTimestamp()
        {
            return esp_log_timestamp();
        }

    }

}
}
