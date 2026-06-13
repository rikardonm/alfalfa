#pragma once

#include <cstdint>
#include <span>
#include <optional>

#include <alfalfa/hal/spi.hpp>
#include <alfalfa/hal/dio.hpp>


namespace alfalfa
{
namespace hal
{
namespace impl
{
namespace bitbang
{
namespace spi
{
    class Module : public ::alfalfa::hal::spi::iModule
    {
    public:
        /**
         * @brief This module provides a bit-bang implementation for an SPI interface.
         *
         * The target device chip select shall be controlled by the caller.
         *
         * @param miso Master In Slave Out pin
         * @param mosi Master Out Slave In pin
         * @param clk Clock pin
         * @param nop_multiplier Multiplier for the number of NOP instructions to insert for timing
         */
        Module(
            std::unique_ptr<::alfalfa::hal::dio::iInput>&& miso,
            std::unique_ptr<::alfalfa::hal::dio::iOutput>&& mosi,
            std::unique_ptr<::alfalfa::hal::dio::iOutput>&& clk,
            uint32_t nop_multiplier = 1);
        void Transceive(const uint8_t data) override;
        void Transceive(const uint8_t data, uint8_t& target) override;
        void Transceive(const std::span<const uint8_t> data) override;
        void Transceive(const std::span<const uint8_t> data, std::span<uint8_t> target) override;
        void Flush() override;

    private:
        std::unique_ptr<::alfalfa::hal::dio::iInput> _miso;
        std::unique_ptr<::alfalfa::hal::dio::iOutput> _mosi;
        std::unique_ptr<::alfalfa::hal::dio::iOutput> _clk;
        const uint32_t _nop_multiplier;

        uint8_t __WriteByte(uint8_t byte);
    };
}
}
}
}
}
