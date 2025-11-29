#pragma once

#include <ratio>
#include <string>
#include <sstream>
#include <type_traits>
#include <exception>
#include <regex>
#include <tuple>
#include <concepts>

#include <alfalfa/lib/errors.hpp>

#include "symbols.hpp"
#include "_unit_base.hpp"
#include "unitless.hpp"


namespace Units
{
    template<PhysicalQuantity UNIT_TYPE, class BASECLASS = _UnitBase>
    class _Unit : public BASECLASS
    {
        public:
            using basetype = _Unit<UNIT_TYPE, BASECLASS>;
            using underlying_type = double;

            /***********************************************************************************************************
             ***************************** Constructors
             **********************************************************************************************************/
            constexpr _Unit()
                : BASECLASS()
            {}

            template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
            constexpr explicit _Unit(int value, std::ratio<Nx, Dx> ratio = {})
                : BASECLASS((static_cast<underlying_type>(value) * Nx) / Dx)
            {
                (void)ratio;
            }

            template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
            constexpr explicit _Unit(unsigned long value, std::ratio<Nx, Dx> ratio = {})
                : BASECLASS((static_cast<underlying_type>(value) * Nx) / Dx)
            {
                (void)ratio;
            }

            template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
            constexpr explicit _Unit(unsigned long long value, std::ratio<Nx, Dx> ratio = {})
                : BASECLASS((static_cast<underlying_type>(value) * Nx) / Dx)
            {
                (void)ratio;
            }

            template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
            constexpr explicit _Unit(double value, std::ratio<Nx, Dx> ratio= {})
                : BASECLASS((static_cast<underlying_type>(value) * Nx) / Dx)
            {
                (void)ratio;
            }

            template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
            constexpr explicit _Unit(long double value, std::ratio<Nx, Dx> ratio = {})
                : BASECLASS((static_cast<underlying_type>(value) * Nx) / Dx)
            {
                (void)ratio;
            }

            /***********************************************************************************************************
             ***************************** Getters
             **********************************************************************************************************/
            static PhysicalQuantity Unit()
            {
                return UNIT_TYPE;
            }

            static const char* Symbol()
            {
                return GetSymbol(Unit());
            }

            /***********************************************************************************************************
             ***************************** ToString
             **********************************************************************************************************/
            inline std::string ToString() const
            {
                return std::to_string(BASECLASS::_value) + Symbol();
            }

            template<typename STREAM=std::stringstream>
            inline void ToStream(STREAM& stream, bool autorange = true) const
            {
                if (autorange)
                {
                    ToStreamAutoranged(stream, BASECLASS::_value, Symbol(), 0);
                }
                else
                {
                    stream << BASECLASS::_value << Symbol();
                }
            }

            template<std::intmax_t Nx = 1, std::intmax_t Dx = 1, typename STREAM=std::stringstream>
            inline void ToStream(STREAM& stream, std::ratio<Nx, Dx> ratio) const
            {
                ToStream(stream, BASECLASS::_value * Dx / Nx, Multiples::GetLetter(ratio), Symbol());
            }

            /***********************************************************************************************************
             ***************************** Arithmetic Operations
             **********************************************************************************************************/
            basetype operator+(const basetype& other) const
            {
                return basetype(BASECLASS::_value + other.BASECLASS::_value);
            }

            basetype operator-(const basetype& other) const
            {
                return basetype(BASECLASS::_value - other.BASECLASS::_value);
            }

            basetype& operator+=(const basetype& other)
            {
                BASECLASS::_value += other.BASECLASS::_value;
                return *this;
            }

            basetype& operator-=(const basetype& other)
            {
                BASECLASS::_value -= other.BASECLASS::_value;
                return *this;
            }

            basetype operator+() const
            {
                return basetype(BASECLASS::_value);
            }

            basetype operator-() const
            {
                return basetype(-BASECLASS::_value);
            }

            /***********************************************************************************************************
             ***************************** Comparison Operations
             **********************************************************************************************************/
            constexpr bool operator<(const basetype& other) const
            {
                return this->UnderlyingValue() < other.UnderlyingValue();
            }

            constexpr bool operator<=(const basetype& other) const
            {
                return this->UnderlyingValue() <= other.UnderlyingValue();
            }

            constexpr bool operator>(const basetype& other) const
            {
                return this->UnderlyingValue() > other.UnderlyingValue();
            }

            constexpr bool operator>=(const basetype& other) const
            {
                return this->UnderlyingValue() >= other.UnderlyingValue();
            }

            constexpr bool operator==(const basetype& other) const
            {
                return this->UnderlyingValue() == other.UnderlyingValue();
            }

            constexpr bool operator!=(const basetype& other) const
            {
                return this->UnderlyingValue() != other.UnderlyingValue();
            }

            /***********************************************************************************************************
             ***************************** Scalar Operations
             **********************************************************************************************************/
            template<typename T>
            basetype operator*(const T& number) const
            requires (std::integral<T> or std::floating_point<T>)
            {
                return basetype(this->UnderlyingValue() * number);
            }

            template<typename T>
            basetype operator*(const T& other) const
            requires Units::UnitlessConcept<T>
            {
                return basetype(this->UnderlyingValue() * other.UnderlyingValue());
            }

            template<typename T>
            basetype operator*=(const T& number) const
            requires (std::integral<T> or std::floating_point<T>)
            {
                BASECLASS::_value *= number;
                return *this;
            }

            template<typename T>
            basetype operator*=(const T& other) const
            requires Units::UnitlessConcept<T>
            {
                BASECLASS::_value *= other.UnderlyingValue();
                return *this;
            }

            basetype operator/(const underlying_type& number) const
            {
                return basetype(BASECLASS::_value / number);
            }

            template<UnitlessSymbol SYMBOL>
            basetype operator/(const _Unitless<SYMBOL>& other) const
            {
                return basetype(BASECLASS::_value / other.UnderlyingValue());
            }

            underlying_type operator/(const basetype& other) const
            {
                return BASECLASS::_value / other.BASECLASS::_value;
            }

            basetype operator/=(const underlying_type& number)
            {
                BASECLASS::_value /= number;
                return *this;
            }

            template<UnitlessSymbol SYMBOL>
            basetype operator/=(const _Unitless<SYMBOL>& other)
            {
                BASECLASS::_value /= other.UnderlyingValue();
                return *this;
            }

        private:

            template<typename STREAM=std::stringstream>
            static inline void ToStream(STREAM& stream, underlying_type value, const char multiple, const char* symbol)
            {
                stream << value << multiple << symbol;
            }

            template<typename STREAM=std::stringstream>
            static inline void ToStreamAutoranged(STREAM& stream, underlying_type value, const char* symbol, int power)
            {
                if (value == 0.0)
                {
                    stream << value << symbol;
                    return;
                }
                while(true)
                {
                    if (abs(value) > 100'000)
                    {
                        value /= 1e3;
                        power += 3;
                    }
                    else if (abs(value) < 0.1)
                    {
                        value *= 1e3;
                        power -= 3;
                    }
                    else
                    {
                        stream << value;
                        if (power != 0)
                        {
                            stream << Multiples::GetLetter(power);
                        }
                        stream << symbol;
                        return;
                    }
                    /* So, for very large numbers, we don't really care anymore... */
                    if (power < -12 or 12 < power)
                    {
                        stream << value << "10e" << power << symbol;
                    }
                }
            }
    };

    /* Guarantee that the new type can be stored in memory with no overhead */
    static_assert(std::is_trivially_copyable_v<_Unit<PhysicalQuantity::Celsius>>);
    static_assert(sizeof(_Unit<PhysicalQuantity::Celsius>) == sizeof(double));

    template<typename T>
    concept UnitConcept = std::is_base_of<Units::_UnitBase, T>::value;

} /* namespace Units */


template<typename T, typename T2>
inline T2 operator*(const T& lhs, const T2& rhs)
requires (not Units::UnitConcept<T> and Units::UnitConcept<T2>)
{
    return rhs * lhs;
}

template<Units::UnitConcept T, typename STREAM=std::ostream>
inline STREAM& operator<<(STREAM& os, const T& unit)
{
    unit.ToStream(os, true);
    return os;
}

template<Units::UnitConcept T>
inline void ToStream(std::stringstream& stream, const T& unit)
{
    unit.ToStream(stream, true);
}

template<Units::UnitConcept T>
inline T FromString(const std::string& value)
{
    // See: https://www.studyplan.dev/pro-cpp/regex
    // See: https://www.studyplan.dev/pro-cpp/regex-capture-groups
    const auto symbol = T::Symbol();
    std::string _units_regex_str(R"(^([-\+]?\d+(?:\.\d+)?)\s?([TGMkmunp]?))");
    _units_regex_str += symbol;
    _units_regex_str += "$";
    const std::regex _units_regex(_units_regex_str);

    std::smatch sm;

    if (not std::regex_match(value, sm, _units_regex))
    {
        throw std::invalid_argument("Wrong number format.");
    }

    const auto numeric_value = sm.str(1);
    const auto mult_str = sm.str(2);

    double mult = 1.0;
    if (not mult_str.empty())
    {
        mult = Units::Multiples::GetMultiplier(mult_str[0]);
    }

    return T(std::stod(numeric_value) * mult);
}

template<Units::UnitConcept T>
struct std::formatter<T>
{
    /* Usage
     *
     * {}
     * {:.2f}
     * {:0.2f}
     * {:.2f}
     * {:3.2f|m}
     * {:.0f|k}
     * {:.2f|M}
     * {:|m}
     * {:.1|p}
     *
     */
    std::optional<std::tuple<char, double>> multiplier_info;
    std::formatter<double> double_fmt;

    constexpr auto parse(std::format_parse_context& ctx)
    {
        auto _first = ctx.begin();
        const auto _last = std::ranges::find(ctx, '}');
        const auto _pipe = std::min(std::ranges::find(ctx, '|'), _last);
        // We create a new view to remove our own format specifiers
        if (_first != _pipe)
        {
            auto ctx2 = std::format_parse_context(std::string_view(_first, _pipe - 1));
            auto ret = double_fmt.parse(ctx2);
            if (_pipe > _last)
            {
                return ret;
            }
        }
        _first = _pipe;
        if (*_first == '}')
        {
            return _first;
        }
        // Remove our pipe indicator, resulting in "x"
        _first += 1;
        if ((_last - _first) != 1)
        {
            throw std::format_error("Invalid multiple specifier");
        }

        auto multiplier = Units::Multiples::GetMultiplier(*_first);
        if (multiplier == 1.0 or multiplier == 0.0)
        {
            throw std::format_error("Invalid multiple specifier");
        }
        multiplier_info = std::make_tuple(*_first, multiplier);

        _first += 1;
        if (*_first != '}')
        {
            throw std::format_error("Unexpected trailing data.");
        }
        return _first;
    }

    auto format(const T& value, std::format_context& ctx) const
    {
        if (multiplier_info)
        {
            auto [letter, mult] = *multiplier_info;
            auto x = double_fmt.format(value.UnderlyingValue() / mult, ctx);
            return std::format_to(x, "{}{}", letter, value.Symbol());
        }
        else
        {
            auto x = double_fmt.format(value.UnderlyingValue(), ctx);
            return std::format_to(x, "{}", value.Symbol());
        }
    }
};
