#pragma once

#include "_base.hpp"
#include "symbols.hpp"


namespace Units
{
    using Volt = _Unit<PhysicalQuantity::Volt>;

    namespace Literals
    {
        #pragma push_macro("_kV")
        #pragma push_macro("_V")
        #pragma push_macro("_mV")
        #pragma push_macro("_uV")
        #undef _kV
        #undef _V
        #undef _mV
        #undef _uV

        constexpr Volt operator""_kV(long double value)
        {
            return Volt(value, std::kilo());
        }

        constexpr Volt operator""_V(long double value)
        {
            return Volt(value);
        }

        constexpr Volt operator""_mV(long double value)
        {
            return Volt(value, std::milli());
        }

        constexpr Volt operator""_uV(long double value)
        {
            return Volt(value, std::micro());
        }

        #pragma pop_macro("_kV")
        #pragma pop_macro("_V")
        #pragma pop_macro("_mV")
        #pragma pop_macro("_uV")
    } /* namespace Literals */


    using Ampere = _Unit<PhysicalQuantity::Ampere>;

    namespace Literals
    {
        constexpr Ampere operator"" _kA(long double value)
        {
            return Ampere(value, std::kilo());
        }

        constexpr Ampere operator""_A(long double value)
        {
            return Ampere(value);
        }

        constexpr Ampere operator""_mA(long double value)
        {
            return Ampere(value, std::milli());
        }

        constexpr Ampere operator""_uA(long double value)
        {
            return Ampere(value, std::micro());
        }
    } /* namespace Literals */


    using Watt = _Unit<PhysicalQuantity::Watt>;

    namespace Literals
    {
        constexpr Watt operator""_kW(long double value)
        {
            return Watt(value, std::kilo());
        }

        constexpr Watt operator""_W(long double value)
        {
            return Watt(value);
        }

        constexpr Watt operator""_mW(long double value)
        {
            return Watt(value, std::milli());
        }

        constexpr Watt operator""_uW(long double value)
        {
            return Watt(value, std::micro());
        }
    } /* namespace Literals */

} /* namespace Units */
