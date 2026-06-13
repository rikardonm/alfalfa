#pragma once

#include <alfalfa/lib/types/units/all.hpp>


namespace alfalfa
{
namespace lib
{
namespace math
{
namespace filters
{
    template<typename T>
    class DiscreteTransform
    {
    public:
        virtual T Next(T input) = 0;
    };

    template<typename T>
    concept MultiplicativeAdditive = requires(T a, T b, double d) {
        { d * a } -> std::convertible_to<T>;
        { b += a };
    };

    template<typename T>
    class PassThrough : public DiscreteTransform<T>
    {
    public:
        T Next(T input) override
        {
            return input;
        }
    };

    template<MultiplicativeAdditive T, std::size_t N>
    class FIR : public DiscreteTransform<T>
    {
    public:
        FIR(std::array<double, N> coefficients)
            : _coefficients(coefficients)
            , _history()
        {
        }

        T Next(T input) override
        {
            // Shift history
            for (std::size_t i = N - 1; i > 0; --i)
            {
                _history[i] = _history[i - 1];
            }
            _history[0] = input;

            // Apply FIR filter
            T output = {};
            for (std::size_t i = 0; i < N; ++i)
            {
                output += _coefficients[i] * _history[i];
            }
            return output;
        }

    private:
        std::array<double, N> _coefficients;
        std::array<T, N> _history;
    };

}
}
}
}
