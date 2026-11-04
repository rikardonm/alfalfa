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
    class Module;

    class Slave : public ::alfalfa::hal::spi::iSlave
    {
    public:
        Slave(
            std::shared_ptr<Module> module,
            std::unique_ptr<::alfalfa::hal::dio::iOutput>&& chip_select_n,
            uint32_t nop_multiplier = 1);
        virtual ~Slave() override = default;
        void Transmit(const std::byte data) override;
        void Transceive(const std::byte data, std::byte& target) override;
        void Receive(std::byte& target) override;
        void Transmit(const std::span<const std::byte> data) override;
        void Transceive(const std::span<const std::byte> data, std::span<std::byte> target) override;
        void Receive(std::span<std::byte> target) override;
    private:
        std::shared_ptr<Module> _module;
        std::unique_ptr<::alfalfa::hal::dio::iOutput> _chip_select_n;
        const uint32_t _nop_multiplier;

        uint8_t __WriteByte(uint8_t byte);
    };

    class Module
    {
    public:
        friend class Slave;
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
            std::unique_ptr<::alfalfa::hal::dio::iOutput>&& clk);

    private:
        std::unique_ptr<::alfalfa::hal::dio::iInput> _miso;
        std::unique_ptr<::alfalfa::hal::dio::iOutput> _mosi;
        std::unique_ptr<::alfalfa::hal::dio::iOutput> _clk;
    };

}
}
}
}
}
