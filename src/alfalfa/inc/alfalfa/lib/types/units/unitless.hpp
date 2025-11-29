#pragma once

#include <ratio>
#include <string>
#include <sstream>
#include <regex>
#include <tuple>
#include <string>
#include <concepts>

#include <alfalfa/lib/errors.hpp>
#include <alfalfa/lib/types/units/symbols.hpp>

#include "_unit_base.hpp"


namespace Units
{
    template<UnitlessSymbol SYMBOL>
    class _Unitless : public _UnitlessBase
    {
    public:
        using underlying_type = double;
        using basetype = _Unitless<SYMBOL>;
        using baseclass = _UnitlessBase;

        /***********************************************************************************************************
         ***************************** Constructors
         **********************************************************************************************************/
        constexpr _Unitless()
            : baseclass()
        {
        }

        constexpr explicit _Unitless(int value)
            : baseclass(static_cast<underlying_type>(value))
        {
        }

        constexpr explicit _Unitless(unsigned long long value)
            : baseclass(static_cast<underlying_type>(value))
        {
        }

        constexpr explicit _Unitless(double value)
            : baseclass(static_cast<underlying_type>(value))
        {
        }

        constexpr explicit _Unitless(long double value)
            : baseclass(static_cast<underlying_type>(value))
        {
        }

        /***********************************************************************************************************
         ***************************** Getters
            **********************************************************************************************************/
        static UnitlessSymbol SymbolType()
        {
            return SYMBOL;
        }

        /***********************************************************************************************************
         ***************************** ToString
         **********************************************************************************************************/
        template <typename STREAM = std::stringstream>
        void ToStream(STREAM &stream) const
        {
            stream << baseclass::_value * Multiples::GetMultiplierUnitless(SymbolType()) << Multiples::GetLettersUnitless(SymbolType());
        }

        std::string ToString() const
        {
            auto ss = std::stringstream();
            ToStream(ss);
            return ss.str();
        }

        /***********************************************************************************************************
         ***************************** Arithmetic Operations
         **********************************************************************************************************/
        template<UnitlessSymbol OTHER_SYMBOL>
        basetype operator+(const _Unitless<OTHER_SYMBOL> &other) const
        {
            return basetype(baseclass::_value + other.UnderlyingValue());
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        basetype operator-(const _Unitless<OTHER_SYMBOL> &other) const
        {
            return basetype(baseclass::_value - other.UnderlyingValue());
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        basetype &operator+=(const _Unitless<OTHER_SYMBOL> &other)
        {
            baseclass::_value += other.UnderlyingValue();
            return *this;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        basetype &operator-=(const _Unitless<OTHER_SYMBOL> &other)
        {
            baseclass::_value -= other.UnderlyingValue();
            return *this;
        }

        basetype operator+() const
        {
            return basetype(baseclass::_value);
        }

        basetype operator-() const
        {
            return basetype(-baseclass::_value);
        }

        /***********************************************************************************************************
         ***************************** Assignment Operations
         **********************************************************************************************************/
        template<UnitlessSymbol OTHER_SYMBOL>
        basetype& operator=(const _Unitless<OTHER_SYMBOL>& other)
        {
            if (&other == this)
            {
                return *this;
            }
            baseclass::_value = other.UnderlyingValue();
            return *this;
        }

        /***********************************************************************************************************
         ***************************** Scalar Operations
         **********************************************************************************************************/
        basetype operator*(const underlying_type& number) const
        {
            return basetype(baseclass::_value * number);
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        basetype operator*(const _Unitless<OTHER_SYMBOL>& other) const
        {
            return basetype(baseclass::_value * other.UnderlyingValue());
        }

        basetype& operator*=(const underlying_type& number)
        {
            baseclass::_value *= number;
            return *this;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        basetype& operator*=(const _Unitless<OTHER_SYMBOL>& other)
        {
            baseclass::_value *= other.UnderlyingValue();
            return *this;
        }

        basetype operator/(const underlying_type& number) const
        {
            return basetype(baseclass::_value / number);
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        basetype operator/(const _Unitless<OTHER_SYMBOL>& other) const
        {
            return basetype(baseclass::_value / other.UnderlyingValue());
        }

        basetype& operator/=(const underlying_type& number)
        {
            baseclass::_value /= number;
            return *this;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        basetype& operator/=(const _Unitless<OTHER_SYMBOL>& other)
        {
            baseclass::_value /= other.UnderlyingValue();
            return *this;
        }

        /***********************************************************************************************************
         ***************************** Logic Operations
         **********************************************************************************************************/
        bool operator>(const underlying_type& number) const
        {
            return baseclass::_value > number;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        bool operator>(const _Unitless<OTHER_SYMBOL>& number) const
        {
            return baseclass::_value > number._value;
        }

        bool operator>=(const underlying_type& number) const
        {
            return baseclass::_value >= number;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        bool operator>=(const _Unitless<OTHER_SYMBOL>& number) const
        {
            return baseclass::_value >= number._value;
        }

        bool operator<(const underlying_type& number) const
        {
            return baseclass::_value < number;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        bool operator<(const _Unitless<OTHER_SYMBOL>& number) const
        {
            return baseclass::_value < number._value;
        }

        bool operator<=(const underlying_type& number) const
        {
            return baseclass::_value <= number;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        bool operator<=(const _Unitless<OTHER_SYMBOL>& number) const
        {
            return baseclass::_value <= number._value;
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        bool operator==(const _Unitless<OTHER_SYMBOL>& other) const
        {
            return baseclass::_value == other.UnderlyingValue();
        }

        template<UnitlessSymbol OTHER_SYMBOL>
        bool operator!=(const _Unitless<OTHER_SYMBOL>& other) const
        {
            return baseclass::_value != other.UnderlyingValue();
        }
    };

    using Unitless = _Unitless<UnitlessSymbol::None>;
    using Percent = _Unitless<UnitlessSymbol::Percent>;
    using Ppm = _Unitless<UnitlessSymbol::Ppm>;
    using Bpm = _Unitless<UnitlessSymbol::Bpm>;

    /* Guarantee that the new type can be stored in memory with no overhead */
    static_assert(std::is_trivially_copyable_v<Bpm>);
    static_assert(sizeof(Bpm) == sizeof(double));

    namespace Literals
    {
        #pragma push_macro("_percent")
        #undef _percent

        constexpr Percent operator""_percent(unsigned long long value)
        {
            return Percent(value / 100.0);
        }

        constexpr Percent operator""_percent(long double value)
        {
            return Percent(value / 100.0);
        }

        #pragma pop_macro("_percent")


        #pragma push_macro("_ppm")
        #undef _ppm

        constexpr Ppm operator""_ppm(unsigned long long value)
        {
            return Ppm(value / 1e6);
        }

        constexpr Ppm operator""_ppm(long double value)
        {
            return Ppm(value / 1e6);
        }

        #pragma pop_macro("_ppm")


        #pragma push_macro("_bpm")
        #undef _bpm

        constexpr Bpm operator""_bpm(unsigned long long value)
        {
            return Bpm(value / 1e6);
        }

        constexpr Bpm operator""_bpm(long double value)
        {
            return Bpm(value / 1e6);
        }

        #pragma pop_macro("_bpm")

    } /* namespace Literals */

    template<typename T>
    concept UnitlessConcept = std::is_base_of<Units::_UnitlessBase, T>::value;

    template <typename STREAM = std::ostream, Units::UnitlessSymbol SYMBOL>
    inline STREAM &operator<<(STREAM &os, const Units::_Unitless<SYMBOL> &value)
    {
        value.ToStream(os);
        return os;
    }

} /* namespace Units */

template<typename T, typename T2>
inline T2 operator*(const T& lhs, const T2& rhs)
requires (std::floating_point<T> and Units::UnitlessConcept<T2>)
{
    return rhs * lhs;
}


template<Units::UnitlessConcept T>
inline T FromString(const std::string& value)
{
    // See: https://www.studyplan.dev/pro-cpp/regex
    // See: https://www.studyplan.dev/pro-cpp/regex-capture-groups
    static const std::regex _units_regex(R"(^([-\+]?\d+(?:\.\d+)?)\s?([%a-zA-Z]{0,6})$)");

    std::smatch sm;

    if (not std::regex_match(value, sm, _units_regex))
    {
        throw std::invalid_argument("Wrong number format.");
    }

    const auto numeric_value = sm.str(1);
    const auto symbol_str = sm.str(2);

    auto symbol = Units::UnitlessSymbol::None;
    if (symbol_str == "%")
    {
        symbol = Units::UnitlessSymbol::Percent;
    }
    else if (symbol_str == "ppm")
    {
        symbol = Units::UnitlessSymbol::Ppm;
    }
    else if (symbol_str == "bpm")
    {
        symbol = Units::UnitlessSymbol::Bpm;
    }
    else if (symbol_str.size() > 0)
    {
        throw std::invalid_argument("Unknown unit.");
    }

    return T(std::stod(numeric_value) / Units::Multiples::GetMultiplierUnitless(symbol));
}

template<Units::UnitlessConcept T>
struct std::formatter<T>
{
    /* Usage
     *
     * {}
     * {:.2f}
     * {:0.2f}
     * {:.2f}
     *
     */
    std::formatter<double> double_fmt;

    constexpr auto parse(std::format_parse_context& ctx)
    {
        return double_fmt.parse(ctx);
    }

    auto format(const T& value, std::format_context& ctx) const
    {
        auto x = double_fmt.format(value.UnderlyingValue() * Units::Multiples::GetMultiplierUnitless(value.SymbolType()), ctx);
        return std::format_to(x, "{}", Units::Multiples::GetLettersUnitless(value.SymbolType()));
    }
};
