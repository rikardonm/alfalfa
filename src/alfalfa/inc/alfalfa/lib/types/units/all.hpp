#pragma once

#include <ranges>
#include <charconv>
#include <type_traits>

#include <nlohmann/json.hpp>

/* Declare all units up-front */
#include <alfalfa/lib/types/units/_unit_base.hpp>
#include <alfalfa/lib/types/units/unitless.hpp>

#include <alfalfa/lib/types/units/date.hpp>
#include <alfalfa/lib/types/units/electrical.hpp>
#include <alfalfa/lib/types/units/mass.hpp>
#include <alfalfa/lib/types/units/pressure.hpp>
#include <alfalfa/lib/types/units/speed.hpp>
#include <alfalfa/lib/types/units/temperature.hpp>
#include <alfalfa/lib/types/units/time.hpp>
#include <alfalfa/lib/types/units/volume.hpp>


/* Type aliases; constants */
namespace Units
{
    using namespace Literals;

    constexpr auto ppm_0 = 0_ppm;

    constexpr auto percent_0 = 0_percent;
    constexpr auto percent_100 = 100_percent;

    /// @brief Clip a value to the range [0, 100]
    inline Percent Clip0to100(Percent value)
    {
        if (value < 0_percent)
        {
            return Units::percent_0;
        }
        else if (value > 100_percent)
        {
            return Units::percent_100;
        }
        return value;
    }

    constexpr auto celsius_0 = 0_C;
    constexpr auto celsius_100 = 100_C;

    template<typename T>
    concept BaseUnitsConcept = UnitConcept<T> or UnitlessConcept<T>;
    // Check out this guy's blog post:
    // https://www.sandordargo.com/blog/2021/03/17/write-your-own-cpp-concepts-part-ii

} /* namespace Units */


/* Now specify the relationship between them */
template<typename T, typename T2>
constexpr inline T operator/(const T& lhs, const T2& rhs)
requires Units::UnitConcept<T> and Units::UnitlessConcept<T2>
{
    return T(lhs.UnderlyingValue() / rhs.UnderlyingValue());
}

template<typename T, typename T2>
constexpr inline T operator/(const T& lhs, const T2& rhs)
requires Units::BaseUnitsConcept<T> and (std::integral<T2> or std::floating_point<T2>)
{
    return T(lhs.UnderlyingValue() / rhs);
}


template<typename T>
constexpr inline std::string ToString(const T& unit)
requires Units::BaseUnitsConcept<T>
{
    auto stream = std::stringstream();
    unit.ToStream(stream);
    return stream.str();
}

namespace Units
{
    template<typename T>
    constexpr inline void to_json(nlohmann::json& json, const T& value)
    requires Units::BaseUnitsConcept<T>
    {
        json = value.ToString();
    }

    template<typename T>
    constexpr inline void from_json(const nlohmann::json& json, T& value)
    requires Units::BaseUnitsConcept<T>
    {
        value = ::FromString<T>(json.get<std::string>());
    }
}
