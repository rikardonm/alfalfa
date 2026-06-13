#pragma once

#include <cstdint>
#include <memory>
#include <alfalfa/hal/dio.hpp>


namespace alfalfa
{
namespace bal
{
    /**
     * @brief Single LED abstraction driven by a digital I/O.
     *
     */
    class LED
    {
    public:
        /**
         * @brief Construct a new LED object.
         *
         * @param out The output pin instance to drive the LED.
         * @param low_side_load If pin is connected to the LED's cathode (in contrast to the annode)
         */
        LED(std::unique_ptr<alfalfa::hal::dio::iOutput>&& out, bool low_side_load = true);

        /**
         * @brief Set the LED to the supplied state.
         *
         * @param state Target state.
         */
        void Set(bool state);

        /**
         * @brief Turn LED on.
         *
         */
        void On();

        /**
         * @brief Turn LED off.
         *
         */
        void Off();

        /**
         * @brief Invert the LED's state.
         *
         */
        void Toggle();

        alfalfa::hal::dio::iOutput& Output() const;

    protected:
        std::unique_ptr<alfalfa::hal::dio::iOutput> _output;

    private:
        bool _last;
        bool _low_side;
    };
}
}
