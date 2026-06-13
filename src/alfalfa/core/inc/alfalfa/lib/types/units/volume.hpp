#pragma once

#include "_base.hpp"
#include "symbols.hpp"


namespace Units
{
    using Liter = _Unit<PhysicalQuantity::Liter>;

    namespace Literals
    {
        #pragma push_macro("_kL")
        #pragma push_macro("_L")
        #pragma push_macro("_mL")
        #undef _kL
        #undef _L
        #undef _mL

        constexpr Liter operator""_kL(long double value)
        {
            return Liter(value, std::kilo());
        }

        constexpr Pascal operator""_kL(unsigned long long value)
        {
            return Pascal(value, std::kilo());
        }

        constexpr Liter operator""_L(long double value)
        {
            return Liter(value);
        }

        constexpr Liter operator""_L(unsigned long long value)
        {
            return Liter(value);
        }

        constexpr Liter operator""_mL(long double value)
        {
            return Liter(value, std::milli());
        }

        constexpr Liter operator""_mL(unsigned long long value)
        {
            return Liter(value, std::milli());
        }

        #pragma pop_macro("_kL")
        #pragma pop_macro("_L")
        #pragma pop_macro("_mL")
    }

} /* namespace Units */
