#pragma once

#include <alfalfa/lib/types/units/all.hpp>

#include <cstddef>
#include <memory>
#include <filesystem>
#include <variant>
#include <span>
#include <mutex>
#include <ranges>


namespace alfalfa
{
namespace lib
{
namespace math
{

    /**
     * @brief This class implements a linear scaling function: y = mx + b
     *
     */
    template<Units::UnitConcept TIN, Units::UnitConcept TOUT>
    class LinearScaler
    {
    public:
        LinearScaler(double m, double b)
            : _m{m}
            , _b{b}
        {
        }
        LinearScaler(Units::Unitless m, Units::Unitless b)
            : _m(m)
            , _b(b)
        {
        }

        TOUT Scale(const TIN& in) const
        {
            return TOUT((_m * in).UnderlyingValue() + _b.UnderlyingValue());
        }

    private:
        Units::Unitless _m;
        Units::Unitless _b;
    };

}
}
}
