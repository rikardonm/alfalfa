#pragma once

#include "_base.hpp"
#include "symbols.hpp"


namespace Units
{
    using MeterPerSecond = _Unit<PhysicalQuantity::MeterPerSecond>;

    namespace Literals
    {
        #pragma push_macro("_m_s")
        #undef _m_s

        constexpr MeterPerSecond operator""_m_s(long double value)
        {
            return MeterPerSecond(value);
        }

        constexpr MeterPerSecond operator""_m_s(unsigned long long value)
        {
            return MeterPerSecond(value);
        }

        #pragma pop_macro("_m_s")
    }

} /* namespace Units */
