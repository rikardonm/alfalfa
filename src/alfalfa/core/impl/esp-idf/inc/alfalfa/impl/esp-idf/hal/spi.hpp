#pragma once

#include <cstdint>

#include <alfalfa/lib/types/units/all.hpp>

#include <alfalfa/hal/spi.hpp>
#include <alfalfa/hal/pin.hpp>

#include <driver/spi_master.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>


namespace alfalfa
{
namespace hal
{
namespace impl
{
namespace spi
{
    class Module;

    class Slave : public alfalfa::hal::spi::iSlave
    {
    public:
        Slave(
            std::shared_ptr<Module> module,
            std::optional<alfalfa::hal::Pin>&& chip_select,
            int clock_speed_hz);
        Slave(
            std::shared_ptr<Module> module,
            std::optional<alfalfa::hal::Pin>&& chip_select,
            Units::Hertz clock_speed_hz);

        ~Slave() override = default;
        void Transmit(const std::byte data) override;
        void Transceive(const std::byte data, std::byte& target) override;
        void Receive(std::byte& target) override;
        void Transmit(const std::span<const std::byte> data) override;
        void Transceive(const std::span<const std::byte> data, std::span<std::byte> target) override;
        void Receive(std::span<std::byte> target) override;
    private:
        std::shared_ptr<Module> _module;
        spi_device_handle_t _dev_handle;
    };

    class Module
    {
    public:
        friend class Slave;

        Module(
            spi_host_device_t host,
            std::optional<::alfalfa::hal::Pin>&& miso,
            std::optional<::alfalfa::hal::Pin>&& mosi,
            std::optional<::alfalfa::hal::Pin>&& clk,
            int max_tx_size);

    private:
        spi_host_device_t _host;
        std::optional<alfalfa::hal::Pin> _miso;
        std::optional<alfalfa::hal::Pin> _mosi;
        std::optional<alfalfa::hal::Pin> _clk;
    };

}
}
}
}
