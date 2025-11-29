#pragma once

#include "_base.hpp"
#include "symbols.hpp"


namespace Units
{
    using Pascal = _Unit<PhysicalQuantity::Pascal>;

    namespace Literals
    {
        #pragma push_macro("_kPa")
        #pragma push_macro("_Pa")
        #pragma push_macro("_mPa")
        #undef _kPa
        #undef _Pa
        #undef _mPa

        constexpr Pascal operator""_kPa(long double value)
        {
            return Pascal(value, std::kilo());
        }

        constexpr Pascal operator""_kPa(unsigned long long value)
        {
            return Pascal(value, std::kilo());
        }

        constexpr Pascal operator""_Pa(long double value)
        {
            return Pascal(value);
        }

        constexpr Pascal operator""_Pa(unsigned long long value)
        {
            return Pascal(value);
        }

        constexpr Pascal operator""_mPa(long double value)
        {
            return Pascal(value, std::milli());
        }

        constexpr Pascal operator""_mPa(unsigned long long value)
        {
            return Pascal(value, std::milli());
        }


        #pragma pop_macro("_kPa")
        #pragma pop_macro("_Pa")
        #pragma pop_macro("_mPa")
    }


} /* namespace Units */
