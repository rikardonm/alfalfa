#pragma once

#include <cstdint>
#include <array>
#include <cmath>


namespace alfalfa
{
namespace lib
{
namespace math
{
    /**
     * @brief Class for rolling average.
     *
     * @tparam T
     * @tparam SIZE
     */
    template <class T, std::size_t SIZE>
    struct RollingAverage
    {
        std::array<T, SIZE> readings;
        uint16_t readIndex = 0;
        T total;

        T smooth(T current_measurement)
        {
            total = total - readings[readIndex];
            readings[readIndex] = current_measurement;
            total = total + readings[readIndex];
            readIndex++;
            if (readIndex >= SIZE)
            {
                readIndex = 0;
            }
            return total / SIZE;
        }
    };

    /**
     * @brief Class for a simple exponential average.
     *
     * @tparam T
     * @tparam SIZE
     */
    template <class T, std::size_t SIZE>
    struct ExponentialAverage
    {
        T average = {};
        double alpha = (1.7 / (SIZE + 1));

        /**
         * @brief Append a new value to the rolling average; returns the new average.
         *
         * @param current_measurement The new point to be taken into account.
         * @return T The new average value.
         */
        T smooth(T current_measurement)
        {
            average = (1 - alpha) * average + alpha * current_measurement;
            return average;
        }
    };

}
}
}
