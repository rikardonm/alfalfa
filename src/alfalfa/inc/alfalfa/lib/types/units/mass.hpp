#pragma once

#include "_base.hpp"
#include "symbols.hpp"


namespace Units
{
    using Gram = _Unit<PhysicalQuantity::Gram>;

    namespace Literals
    {
        #pragma push_macro("_g")
        #pragma push_macro("_kg")
        #undef _g
        #undef _kg

        constexpr Gram operator""_g(long double value)
        {
            return Gram(value);
        }

        constexpr Gram operator""_g(unsigned long long value)
        {
            return Gram(value);
        }

        constexpr Gram operator""_kg(long double value)
        {
            return Gram(value, std::kilo());
        }

        constexpr Gram operator""_kg(unsigned long long value)
        {
            return Gram(value, std::kilo());
        }

        #pragma pop_macro("_g")
        #pragma pop_macro("_kg")
    }

} /* namespace Units */
