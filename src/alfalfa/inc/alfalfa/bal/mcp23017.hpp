#pragma once

/*
 * Classes that provide control over the MCP23017 I2C I/O expander.
 * Based on:
 *      https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/tree/master
 *      https://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf
 */

#include <cstdint>
#include <array>
#include <variant>
#include <cstddef>

#include <alfalfa/hal/pin.hpp>
#include <alfalfa/hal/dio.hpp>
#include <alfalfa/hal/two_wire.hpp>


namespace alfalfa
{
namespace bal
{
namespace MCP23017
{
    const uint8_t bus_width = 0x08;
    using BusData = std::array<bool, bus_width>;

    enum class Port
    {
        A = 0x00,
        B = 0x01,
    };

    namespace Registers
    {
        struct IOCON
        {
            /// @brief INTPOL - This bit sets the polarity of the INT output pin. 0 = Active-low. 1 = Active-high.
            bool intpol = false;
            /// ODR: This bit configures the INT pin as an open-drain output. 0 = Active driver output (INTPOL bit sets the polarity). 1 = Open-drain output (overrides the INTPOL bit).
            bool odr = false;
            /// @brief HAEN - Hardware Address Enable bit (MCP23S17 only). Address pins are always enabled on MCP23017.
            bool haen = false;
            /// @brief DISSLW - Slew Rate control bit for SDA output. 1 = Slew rate disabled. 0 = Slew rate enabled.
            bool disslew = false;
            /// @brief SEQOP - Sequential Operation mode bit. 1 = Sequential operation disabled, address pointer does not increment. 0 = Sequential operation enabled, address pointer increments.
            bool seqop = false;
            /// @brief MIRROR - INT Pins Mirror bit. 1 = The INT pins are internally connected. 0 = The INT pins are not connected. INTA is associated with PortA and INTB is associated with PortB.
            bool mirror = false;
            /// @brief BANK - Controls how the registers are addressed. 1 = The registers associated with each port are separated into different banks. 0 = The registers are in the same bank (addresses are sequential).
            bool bank = false;

            uint8_t ToByte() const
            {
                return (intpol << 1) | (odr << 2) | (haen << 3) | (disslew << 4) | (seqop << 5) | (mirror << 6) | (bank << 7);
            };
        };
    } /* namespace Registers */

    class _Base
    {
    public:
        _Base(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> i2c_module,
            const bool a2,
            const bool a1,
            const bool a0,
            std::optional<std::shared_ptr<alfalfa::hal::dio::iOutput>> reset_n = {});

        void ReInit();

    protected:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _wire;
        std::optional<std::shared_ptr<alfalfa::hal::dio::iOutput>> _reset_n;
        const std::byte _ax_address;
    };

    class UniformBusOutput : public _Base
    {
    public:
        using _Base::_Base;

        void Configure(const Port port, alfalfa::hal::dio::OutputConfiguration config);
        void Write(const Port port, const BusData& values);
    };

}
}
}
