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
        A,
        B,
    };

    enum class RegisterMap : uint8_t
    {
        IODIRA   = 0x00,
        IODIRB   = 0x01,
        IPOLA    = 0x02,
        IPOLB    = 0x03,
        GPINTENA = 0x04,
        GPINTENB = 0x05,
        DEFVALA  = 0x06,
        DEFVALB  = 0x07,
        INTCONA  = 0x08,
        INTCONB  = 0x09,
        IOCON    = 0x0A,
        GPPUA    = 0x0C,
        GPPUB    = 0x0D,
        INTFA    = 0x0E,
        INTFB    = 0x0F,
        INTCAPA  = 0x10,
        INTCAPB  = 0x11,
        GPIOA    = 0x12,
        GPIOB    = 0x13,
        OLATA    = 0x14,
        OLATB    = 0x15,
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

            std::byte ToByte() const
            {
                return
                    std::byte{0x00}
                    | (intpol ? std::byte{0x02} : std::byte{0x00})
                    | (odr ? std::byte{0x04} : std::byte{0x00})
                    | (haen ? std::byte{0x08} : std::byte{0x00})
                    | (disslew ? std::byte{0x10} : std::byte{0x00})
                    | (seqop ? std::byte{0x20} : std::byte{0x00})
                    | (mirror ? std::byte{0x40} : std::byte{0x00})
                    | (bank ? std::byte{0x80} : std::byte{0x00})
                ;
            };
        };
    } /* namespace Registers */

    struct Base
    {
        Base(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> i2c_module,
            const bool a2,
            const bool a1,
            const bool a0,
            std::optional<std::shared_ptr<alfalfa::hal::dio::iOutput>> reset_n = {});
        void ReInit();

        void WriteRegister(const RegisterMap reg, const std::byte value);
        void WriteRegisterMasked(const RegisterMap reg, const std::byte value, const std::byte mask);
        std::byte ReadRegister(const RegisterMap reg);
        std::byte ReadRegisterMasked(const RegisterMap reg, const std::byte mask);

        void ReadSpan(const RegisterMap reg, std::span<std::byte> out_value);
        void WriteSpan(const RegisterMap reg, const std::span<const std::byte> value);

        template<std::size_t N>
        std::array<std::byte, N> ReadArray(const RegisterMap reg)
        {
            std::array<std::byte, N> ret{};
            ReadSpan(reg, std::span<std::byte>(ret.data(), N));
            return ret;
        }

        void FlushCacheAndRead();

        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _wire;
        std::optional<std::shared_ptr<alfalfa::hal::dio::iOutput>> _reset_n;
        const std::byte _ax_address;

        std::array<std::byte, 0x16> _register_cache;
        bool _immediate_trx;

        struct _Flusher
        {
            _Flusher(Base& parent)
                : _parent(parent)
            {
                _parent._immediate_trx = false;
            }
            ~_Flusher()
            {
                _parent._immediate_trx = true;
                _parent.FlushCacheAndRead();
            }
            Base& _parent;
        };

        _Flusher DelayFlush()
        {
            return _Flusher(*this);
        }
    };

    struct UniformBusOutput
    {
        UniformBusOutput(
            std::shared_ptr<Base> base,
            const Port port);
        void Configure(alfalfa::hal::dio::OutputConfiguration config);
        void Write(const BusData& values);

        const Port port;
        std::shared_ptr<Base> base;
    };

    struct OutputPin : public alfalfa::hal::dio::iOutput
    {
        OutputPin(
            std::shared_ptr<Base> base,
            const Port port,
            const uint8_t index);
        virtual void Configure(alfalfa::hal::dio::OutputConfiguration config) override;
        virtual void Write(bool value) const override;

        const Port _port;
        const std::byte _mask;
        std::shared_ptr<Base> _base;
    };

    struct InputPin : public alfalfa::hal::dio::iInput
    {
        InputPin(
            std::shared_ptr<Base> base,
            const Port port,
            const uint8_t index);
        virtual void Configure(alfalfa::hal::dio::InputConfiguration config) override;
        virtual bool Read() const override;

        const Port _port;
        const std::byte _mask;
        std::shared_ptr<Base> _base;
    };

}
}
}
