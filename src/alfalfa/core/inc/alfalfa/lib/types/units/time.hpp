#pragma once

#include <cstdint>
#include <ctime>
#include <sstream>
#include <cmath>

#include <nlohmann/json.hpp>
#include <alfalfa/lib/types/json.hpp>
#include <alfalfa/lib/types/nlohmann_extensions.hpp>

#include "_base.hpp"
#include "symbols.hpp"


namespace Units
{
    struct _Second : public _UnitBase
    {
        using _UnitBase::_UnitBase;

        template<typename T>
        time_t To_time_t(const T multiplier, const T divider = 1) const
        {
            return DecayTo<time_t>(multiplier, divider);
        }

        template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
        time_t To_time_t(const std::ratio<Nx, Dx> ratio = {}) const
        {
            (void) ratio;
            return To_time_t(Dx, Nx);
        }
    };

    using Second = _Unit<PhysicalQuantity::Second, _Second>;

    struct TimeDuration
    {
        uint32_t microseconds = 0;
        uint8_t seconds = 0;
        uint8_t minutes = 0;
        uint8_t hours = 0;
        uint16_t days = 0;

        static TimeDuration FromMicroseconds(const uint64_t microseconds)
        {
            auto obj = TimeDuration();
            obj.microseconds = microseconds % 1'000'000;
            auto seconds = microseconds / 1'000'000;
            obj.seconds = seconds % 60;
            auto minutes = seconds / 60;
            obj.minutes = minutes % 60;
            auto hours = minutes / 60;
            obj.hours = hours % 24;
            obj.days = hours / 24;
            return obj;
        }

        operator bool() const
        {
            return microseconds || seconds || minutes || hours || days;
        }
    };

    struct _Hertz : public _UnitBase
    {
        using _UnitBase::_UnitBase;

        static Units::_Hertz FromPeriod(const Units::Second& period)
        {
            return Units::_Hertz(1.0 / period.UnderlyingValue());
        }

        Units::Second GetPeriod() const
        {
            return Units::Second(1) / _value;
        }
    };

    using Hertz = _Unit<PhysicalQuantity::Hertz, _Hertz>;
}


inline std::string ToString(const Units::TimeDuration& duration)
{
    std::string ret;
    ret.reserve(3 + 3 + 3 + 3 + 7);
    ret += std::to_string(duration.days) + 'd';
    ret += std::to_string(duration.hours) + ':';
    ret += std::to_string(duration.minutes) + ':';
    ret += std::to_string(duration.seconds) + '.';
    ret += std::to_string(duration.microseconds);
    return ret;
}

namespace Units
{
    struct HourMinute
    {
        constexpr HourMinute() : hour(0), minute(0)
        {}

        constexpr HourMinute(const uint8_t hour, const uint8_t minute) : hour(hour), minute(minute)
        {}

        constexpr HourMinute(const ::tm& date)
        : hour(static_cast<uint8_t>(date.tm_hour)), minute(static_cast<uint8_t>(date.tm_min))
        {}

        void FromBase100(const uint16_t value)
        {
            hour = value / 100;
            minute = value % 100;
        }

        uint16_t ToBase100() const
        {
            uint16_t ret = hour;
            ret *= 100;
            ret += minute;
            return ret;
        }

        static Units::HourMinute DecodeOnBase100(const uint16_t value)
        {
            auto ret = Units::HourMinute();
            ret.FromBase100(value);
            return ret;
        }

        static uint16_t EncodeOnBase100(const Units::HourMinute& value)
        {
            return value.ToBase100();
        }

        bool operator<(const HourMinute& rhs) const
        {
            if (hour < rhs.hour)
            {
                return true;
            }
            if (hour > rhs.hour)
            {
                return false;
            }
            return minute < rhs.minute;
        }

        bool operator>(const HourMinute& rhs) const
        {
            if (hour > rhs.hour)
            {
                return true;
            }
            if (hour < rhs.hour)
            {
                return false;
            }
            return minute > rhs.minute;
        }

        bool operator==(const HourMinute& rhs) const
        {
            return hour == rhs.hour && minute == rhs.minute;
        }

        bool operator!=(const HourMinute& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<=(const HourMinute& rhs) const
        {
            return *this < rhs || *this == rhs;
        }

        bool operator>=(const HourMinute& rhs) const
        {
            return *this > rhs || *this == rhs;
        }

        bool operator==(const ::tm& rhs) const
        {
            return hour == rhs.tm_hour and minute == rhs.tm_min;
        }

        uint8_t hour;
        uint8_t minute;
    };
}

inline void ToStream(std::stringstream& stream, const Units::HourMinute& daytime)
{
    stream << static_cast<uint16_t>(daytime.hour) << ':' << static_cast<uint16_t>(daytime.minute);
}

inline void ToJson(std::stringstream& stream, const Units::HourMinute& daytime)
{
    stream << '"';
    ToStream(stream, daytime);
    stream << '"';
}

inline alfalfa::ErrorCode FromString(std::string sview, Units::HourMinute& daytime)
{
    if (sview.size() < 4 or sview.size() > 5)
    {
        return alfalfa::ErrorCode::InvalidValue;
    }
    auto pos = sview.find(':');
    int hour = 0;
    int minute = 0;
    if (pos != std::string::npos)
    {
        hour = std::atoi(sview.substr(0, pos).data());
        minute = std::atoi(sview.substr(pos + 1).data());
    }
    else
    {
        auto x = std::atoi(sview.data());
        auto y = Units::HourMinute::DecodeOnBase100(x);
        hour = y.hour;
        minute = y.minute;
    }

    if (hour < 0 or hour > 23 or minute < 0 or minute > 59)
    {
        return alfalfa::ErrorCode::InvalidValue;
    }
    daytime.hour = static_cast<uint8_t>(hour);
    daytime.minute = static_cast<uint8_t>(minute);
    return alfalfa::ErrorCode::None;
}

inline std::string ToString(const Units::HourMinute& daytime, bool use_separator = true)
{
    std::string ret;
    if (daytime.hour < 10)
    {
        ret += '0';
    }
    ret += std::to_string(daytime.hour);
    /* Simplest way to add a leading zero :) */
    if (use_separator)
    {
        ret += ':';
    }
    if (daytime.minute < 10)
    {
        ret += '0';
    }
    ret += std::to_string(daytime.minute);
    return ret;
}

inline void StreamJson(StreamJsonObject& dst, const Units::HourMinute& value)
{
    auto ret = ToString(value);
    dst.Stream(ret);
}

namespace Units
{

    inline void to_json(nlohmann::json& j, const HourMinute& value)
    {
        j = ToString(value);
    }

    inline void from_json(const nlohmann::json& j, HourMinute& value)
    {
        auto ret = ::FromString(j.get<std::string>(), value);
        if (ret != alfalfa::ErrorCode::None)
        {
            throw std::runtime_error("Invalid value");
        }
    }

    struct HourMinuteSecond : public HourMinute
    {
        uint8_t second;

        HourMinuteSecond() : HourMinute(), second(0)
        {};

        HourMinuteSecond(const uint8_t hour, const uint8_t minute, const uint8_t second)
        : HourMinute(hour, minute), second(second)
        {}

        HourMinuteSecond(const ::tm& date)
        : HourMinute(date), second(static_cast<uint8_t>(date.tm_sec))
        {}

        HourMinuteSecond operator-=(const TimeDuration& duration)
        {
            if (second < duration.seconds)
            {
                second += 60;
                minute--;
            }
            second -= duration.seconds;
            if (minute < duration.minutes)
            {
                minute += 60;
                hour--;
            }
            minute -= duration.minutes;
            if (hour < duration.hours)
            {
                hour += 24;
            }
            hour -= duration.hours;
            // days are not considered, as they will wrap anyway
            return *this;
        }

        HourMinuteSecond& operator+=(const TimeDuration& duration)
        {
            second += duration.seconds;
            if (second >= 60)
            {
                second -= 60;
                minute++;
            }
            minute += duration.minutes;
            if (minute >= 60)
            {
                minute -= 60;
                hour++;
            }
            hour += duration.hours;
            if (hour >= 24)
            {
                hour -= 24;
            }
            // days are not considered, as they will wrap anyway
            return *this;
        }

        HourMinuteSecond operator+(const TimeDuration& duration) const
        {
            auto ret = *this;
            ret += duration;
            return ret;
        }

        HourMinuteSecond operator-(const TimeDuration& duration) const
        {
            auto ret = *this;
            ret -= duration;
            return ret;
        }

        bool operator==(const ::tm& rhs) const
        {
            return HourMinute::operator==(rhs) and second == rhs.tm_sec;
        }

        bool operator==(const HourMinuteSecond& rhs) const
        {
            return HourMinute::operator==(rhs) and second == rhs.second;
        }
    };

}

inline std::string ToString(const Units::HourMinuteSecond& daytime)
{
    auto ret = ToString(static_cast<const Units::HourMinute&>(daytime));
    ret += ':';
    if (daytime.second < 10)
    {
        ret += '0';
    }
    ret += std::to_string(daytime.second);
    return ret;
}

inline void StreamJson(StreamJsonObject& dst, const Units::HourMinuteSecond& value)
{
    auto ret = ToString(value);
    dst.Stream(ret);
}

namespace Units
{

    inline void to_json(nlohmann::json& j, const HourMinuteSecond& value)
    {
        j = ToString(value);
    }

    inline void from_json(const nlohmann::json& j, HourMinuteSecond& value)
    {
        auto ret = ::FromString(j.get<std::string>(), value);
        if (ret != alfalfa::ErrorCode::None)
        {
            throw std::runtime_error("Invalid value");
        }
    }

    /**
     * @brief This class represents an UTC offset.
     * This offset is not a time zone, but a difference from UTC.
     * The offset is stored in hours and minutes.
     * Since the difference is allowed to be negative, the class also stores a flag that indicates if the offset is.
     */
    struct UtcOffset
    {
        HourMinute offset;
        /// @brief Flag that, if true, indicates that the stored offset lags UTC (to the west).
        bool runs_behind;

        UtcOffset()
        {}

        UtcOffset(const uint8_t hour, const uint8_t minute, const bool runs_behind)
        : offset(hour, minute), runs_behind(runs_behind)
        {};

        UtcOffset(const HourMinute& hm, const bool runs_behind)
        : offset(hm), runs_behind(runs_behind)
        {};

        UtcOffset(const ::tm& c_date);

        bool operator==(const UtcOffset& rhs) const
        {
            return offset == rhs.offset && runs_behind == rhs.runs_behind;
        }
    };
}

inline std::string ToString(const Units::UtcOffset& value, bool use_separator = false)
{
    std::string ret;
    ret.reserve(use_separator ? 6 : 5);
    ret += value.runs_behind ? '-' : '+';
    ret += ToString(value.offset, use_separator);
    return ret;
}

inline alfalfa::ErrorCode FromString(std::string sview, Units::UtcOffset& offset)
{
    if (sview.size() > 6 or sview.size() < 5)
    {
        return alfalfa::ErrorCode::InvalidValue;
    }
    auto utc_offset = std::string(sview);
    auto ret = FromString(sview.substr(1), offset.offset);
    if (ret == alfalfa::ErrorCode::None)
    {
        offset.runs_behind = utc_offset[0] == '-';
    }
    return ret;
}

inline Units::UtcOffset::UtcOffset(const ::tm& c_date)
{
    std::array<char, 6> utc_offset;
    // It's ugly, yes.
    std::strftime(utc_offset.begin(), utc_offset.size(), "%z", &c_date);
    ::FromString(std::string(utc_offset.begin(), utc_offset.size()), *this);
}

inline void StreamJson(StreamJsonObject& dst, const Units::UtcOffset& value)
{
    auto ret = ToString(value, false);
    dst.Stream(ret);
}

namespace Units
{

    inline void to_json(nlohmann::json& j, const UtcOffset& value)
    {
        j = ToString(value);
    }

    inline void from_json(const nlohmann::json& j, UtcOffset& value)
    {
        auto ret = ::FromString(j.get<std::string>(), value);
        if (ret != alfalfa::ErrorCode::None)
        {
            throw std::runtime_error("Invalid value");
        }
    }

    /**
     * @brief This class represents a date, with day, month and year.
     */
    struct DateType
    {
        uint8_t day;
        // Month starts with 1 at Jan.
        uint8_t month;
        // Year is absolute (with 1900)
        uint16_t year;

        /**
         * @brief Default constructor, which provides a date of 2022-12-22.
         * This is because this class is used to convert to and from `::tm`, and by default that
         * class' year is based on 1900; therefore, we need a valid value for a default-constructed
         * object to be converted into a `::tm` object.
         */
        DateType()
            : day(22)
            , month(12)
            , year(2022)
        {
        }

        DateType(const uint16_t year, const uint8_t month, const uint8_t day)
            : day(day)
            , month(month)
            , year(year)
        {
        }

        DateType(const ::tm& date)
            : day(static_cast<uint8_t>(date.tm_mday))
            , month(static_cast<uint8_t>(date.tm_mon + 1))
            , year(static_cast<uint16_t>(date.tm_year))
        {
        }

        bool operator<(const DateType& rhs)
        {
            if (year < rhs.year)
            {
                return true;
            }
            if (year > rhs.year)
            {
                return false;
            }
            if (month < rhs.month)
            {
                return true;
            }
            if (month > rhs.month)
            {
                return false;
            }
            if (day < rhs.day)
            {
                return true;
            }
            if (day > rhs.day)
            {
                return false;
            }
            return false;
        }

        bool operator==(const DateType& rhs)
        {
            return year == rhs.year && month == rhs.month && day == rhs.day;
        }

        bool operator!=(const DateType& rhs)
        {
            return !(*this == rhs);
        }

        bool operator<=(const DateType& rhs)
        {
            return *this < rhs || *this == rhs;
        }

        bool operator>(const DateType& rhs)
        {
            return !(*this <= rhs);
        }

        bool operator>=(const DateType& rhs)
        {
            return *this > rhs || *this == rhs;
        }
    };
}

inline std::string ToString(const Units::DateType& value)
{
    std::string ret;
    ret.reserve(4 + 1 + 2 + 1 + 2 + 1);
    ret += std::to_string(value.year);
    ret += '-';
    if (value.month < 10)
    {
        ret += '0';
    }
    ret += std::to_string(value.month);
    ret += '-';
    if (value.day < 10)
    {
        ret += '0';
    }
    ret += std::to_string(value.day);
    return ret;
}

inline alfalfa::ErrorCode FromString(std::string sview, Units::DateType& value)
{
    if (sview.size() != 10)
    {
        return alfalfa::ErrorCode::InvalidValue;
    }
    auto pos1 = sview.find('-');
    auto pos2 = sview.find('-', pos1 + 1);
    if (pos1 == std::string::npos || pos2 == std::string::npos)
    {
        return alfalfa::ErrorCode::InvalidValue;
    }
    value.year = std::atoi(sview.substr(0, pos1).data());
    value.month = std::atoi(sview.substr(pos1 + 1, pos2 - pos1 - 1).data());
    value.day = std::atoi(sview.substr(pos2 + 1).data());
    if (value.month < 1 || value.month > 12 || value.day < 1 || value.day > 31)
    {
        return alfalfa::ErrorCode::InvalidValue;
    }
    return alfalfa::ErrorCode::None;
}

inline void StreamJson(StreamJsonObject& dst, const Units::DateType& value)
{
    dst.Stream(ToString(value));
}

namespace Units
{

    inline void to_json(nlohmann::json& j, const Units::DateType& value)
    {
        j = ToString(value);
    }

    inline void from_json(const nlohmann::json& j, DateType& value)
    {
        auto ret = ::FromString(j.get<std::string>(), value);
        if (ret != alfalfa::ErrorCode::None)
        {
            throw std::runtime_error("Invalid value");
        }
    }

    struct LongFormatTimestamp
    {
        HourMinuteSecond time;
        DateType date;
        UtcOffset utc_offset;

        LongFormatTimestamp()
        {}

        LongFormatTimestamp(const DateType& date, const HourMinuteSecond& time, const UtcOffset& utc_offset)
            : time(time), date(date), utc_offset(utc_offset)
        {
        }

        LongFormatTimestamp(const ::tm& c_date)
            : time(c_date), date(c_date), utc_offset(c_date)
        {
        }

        void operator=(const ::tm& c_date)
        {
            this->time = {c_date};
            this->date = {c_date};
            this->utc_offset = {c_date};
        }
    };
}

inline void StreamJson(StreamJsonObject& dst, const Units::LongFormatTimestamp& value)
{
    auto x = StreamJsonObject(dst);
    x.Stream("date", value.date);
    x.Stream("time", value.time);
    x.Stream("utc_offset", value.utc_offset);
}

namespace Units
{

    inline void to_json(nlohmann::json& j, const Units::LongFormatTimestamp& value)
    {
        j["date"] = value.date;
        j["time"] = value.time;
        j["utc_offset"] = value.utc_offset;
    }

    inline void from_json(const nlohmann::json& j, Units::LongFormatTimestamp& value)
    {
        j.at("date").get_to(value.date);
        j.at("time").get_to(value.time);
        j.at("utc_offset").get_to(value.utc_offset);
    }

    namespace Literals
    {
        #pragma push_macro("_s")
        #pragma push_macro("_ms")
        #pragma push_macro("_us")
        #pragma push_macro("_ns")
        #undef _s
        #undef _ms
        #undef _us
        #undef _ns

        constexpr Second operator""_s(unsigned long long value)
        {
            return Second(value);
        }

        constexpr Second operator""_s(long double value)
        {
            return Second(value);
        }

        constexpr Second operator""_ms(unsigned long long value)
        {
            return Second(value, std::milli());
        }

        constexpr Second operator""_ms(long double value)
        {
            return Second(value, std::milli());
        }

        constexpr Second operator""_us(unsigned long long value)
        {
            return Second(value, std::micro());
        }

        constexpr Second operator""_us(long double value)
        {
            return Second(value, std::micro());
        }

        constexpr Second operator""_ns(unsigned long long value)
        {
            return Second(value, std::nano());
        }

        constexpr Second operator""_ns(long double value)
        {
            return Second(value, std::nano());
        }

        #pragma pop_macro("_s")
        #pragma pop_macro("_ms")
        #pragma pop_macro("_us")
        #pragma pop_macro("_ns")


        #pragma push_macro("_Hz")
        #pragma push_macro("_kHz")
        #pragma push_macro("_MHz")
        #pragma push_macro("_GHz")
        #pragma push_macro("_THz")
        #undef _Hz
        #undef _kHz
        #undef _MHz
        #undef _GHz
        #undef _THz

        constexpr Hertz operator""_Hz(unsigned long long value)
        {
            return Hertz(value);
        }

        constexpr Hertz operator""_Hz(long double value)
        {
            return Hertz(value);
        }

        constexpr Hertz operator""_kHz(unsigned long long value)
        {
            return Hertz(value, std::kilo());
        }

        constexpr Hertz operator""_kHz(long double value)
        {
            return Hertz(value, std::kilo());
        }

        constexpr Hertz operator""_MHz(unsigned long long value)
        {
            return Hertz(value, std::mega());
        }

        constexpr Hertz operator""_MHz(long double value)
        {
            return Hertz(value, std::mega());
        }

        constexpr Hertz operator""_GHz(unsigned long long value)
        {
            return Hertz(value, std::giga());
        }

        constexpr Hertz operator""_GHz(long double value)
        {
            return Hertz(value, std::giga());
        }

        constexpr Hertz operator""_THz(unsigned long long value)
        {
            return Hertz(value, std::tera());
        }

        constexpr Hertz operator""_THz(long double value)
        {
            return Hertz(value, std::tera());
        }

        #pragma pop_macro("_Hz")
        #pragma pop_macro("_kHz")
        #pragma pop_macro("_MHz")
        #pragma pop_macro("_GHz")
        #pragma pop_macro("_THz")
    } /* namespace Literals */

} /* namespace Units */


inline std::string ToString(::tm ntm)
{
    char buff[50] = {0};
    ntm.tm_year -= 1900;
    std::strftime(buff, sizeof(buff), "%FT%T%z", &ntm);
    return {buff};
}

inline void to_json(nlohmann::json& j, const ::tm& ntm)
{
    j = ToString(ntm);
}

inline void from_json(const nlohmann::json& j, ::tm& ntm)
{
    auto str = j.get<std::string>();
    auto ret = strptime(str.c_str(), "%FT%T%z", &ntm);
    ntm.tm_year += 1900;
    if (ret == nullptr)
    {
        throw std::runtime_error("Invalid value");
    }
}
