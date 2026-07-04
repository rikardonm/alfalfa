#pragma once

#include <cstdint>
#include <optional>
#include <memory>
#include <span>
#include <array>
#include <ranges>

#include <alfalfa/hal/two_wire.hpp>
#include <alfalfa/lib/types/color.hpp>


namespace alfalfa
{
namespace bal
{
namespace WS2812B
{

    /**
     * @brief A class-bound buffer, single WS2812B LED driver.
     *
     * This driver uses a static (to the class) buffer, avoiding thus using the stack
     * when using large daisy chains.
     */
    template<std::size_t LEN>
    class StaticSpiDriver
    {
    public:
        StaticSpiDriver(std::unique_ptr<alfalfa::hal::spi::iSlave>&& slave)
            : _slave(std::move(slave))
            , _buffer()
        {}

        void SetColor(const std::span<alfalfa::lib::Color, LEN> colors)
        {
            auto buf_span = std::span(_buffer);
            for (auto i = 0; i < LEN; ++i)
            {
                const auto offset = (i * 12);
                // A cast is needed here because, for some reason
                // it seems that the size is erased away from the type
                auto bsp = static_cast<std::span<std::byte, 12>>(buf_span.subspan(offset, 12));
                SetColorOnBuffer(bsp, colors[i]);
            }
            _slave->Transmit(_buffer);
        }

    protected:
        static void SetColorOnBuffer(std::span<std::byte, 12> to_tx, const alfalfa::lib::Color& color)
        {
            // the chip accepts the color as GRB
            to_tx[0 + 0] = MakeByte(0x80, color.green);
            to_tx[0 + 1] = MakeByte(0x20, color.green);
            to_tx[0 + 2] = MakeByte(0x08, color.green);
            to_tx[0 + 3] = MakeByte(0x02, color.green);
            to_tx[4 + 0] = MakeByte(0x80, color.red);
            to_tx[4 + 1] = MakeByte(0x20, color.red);
            to_tx[4 + 2] = MakeByte(0x08, color.red);
            to_tx[4 + 3] = MakeByte(0x02, color.red);
            to_tx[8 + 0] = MakeByte(0x80, color.blue);
            to_tx[8 + 1] = MakeByte(0x20, color.blue);
            to_tx[8 + 2] = MakeByte(0x08, color.blue);
            to_tx[8 + 3] = MakeByte(0x02, color.blue);
        }

    private:
        std::unique_ptr<alfalfa::hal::spi::iSlave> _slave;
        // RGB, 8 bits per channel, 3 bits per channel bit
        std::array<std::byte, LEN * 3 * 4> _buffer = {};

        static std::byte MakeByte(const uint8_t mask, const uint8_t value)
        {
            const auto v =
                ((value & mask) ? 0b1110'0000 : 0b1000'0000)
                | ((value & (mask  >> 1)) ? 0b0000'1110 : 0b0000'1000);
            return std::byte{static_cast<uint8_t>(v)};
        }

    };

}

}
}
