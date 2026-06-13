#pragma once

#include <cmath>


namespace Units
{
    enum class _UnitTypes
    {
        Unit,
        Unitless,
    };

    template<_UnitTypes TYPE>
    struct __UnitsBase
    {
    public:
        constexpr __UnitsBase() : _value() {}
        constexpr __UnitsBase(double value) : _value(value) {}
        constexpr double UnderlyingValue() const
        {
            return _value;
        }

        template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
        double DecayTo(const std::ratio<Nx, Dx> ratio = {}) const
        {
            return _value * (static_cast<double>(Dx) / static_cast<double>(Nx));
        }

        template<typename T>
        T DecayTo(const double multiplier, const double divider) const
        {
            return static_cast<T>(std::lround((_value * multiplier) / divider));
        }

        float DecayToFloat() const
        {
            return static_cast<float>(_value);
        }

        template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
        void RoundAt(const std::ratio<Nx, Dx> ratio = {})
        {
            auto new_value = _value * (static_cast<double>(Dx) / static_cast<double>(Nx));
            new_value = std::round(new_value);
            _value = new_value * (static_cast<double>(Nx) / static_cast<double>(Dx));
        }

    protected:
        /* The value is always stored in the ratio 1:1, without (sub)multiple, that is: gram, meter, ... */
        double _value;
    };

    using _UnitBase = __UnitsBase<_UnitTypes::Unit>;
    using _UnitlessBase = __UnitsBase<_UnitTypes::Unitless>;

} /* namespace Units */
