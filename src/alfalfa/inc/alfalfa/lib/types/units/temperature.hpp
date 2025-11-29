#pragma once

#include "_base.hpp"
#include "symbols.hpp"


namespace Units
{
    using Celsius = _Unit<PhysicalQuantity::Celsius>;

    namespace Literals
    {
        #pragma push_macro("_C")
        #undef _C

        constexpr Celsius operator""_C(unsigned long long value)
        {
            return Celsius(value);
        }

        constexpr Celsius operator""_C(long double value)
        {
            return Celsius(value);
        }

        #pragma pop_macro("_C")
    } /* namespace Literals */

} /* namespace Units */
