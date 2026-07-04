#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <memory>


namespace alfalfa
{
namespace hal
{
namespace spi
{

    /**
     * @brief An abstraction for a slave device connected to an SPI bus.
     *
     */
    class iSlave
    {
    public:
        virtual ~iSlave() = default;
        virtual void Transmit(const std::byte data) = 0;
        virtual void Transceive(const std::byte data, std::byte& target) = 0;
        virtual void Receive(std::byte& target) = 0;
        virtual void Transmit(const std::span<const std::byte> data) = 0;
        virtual void Transceive(const std::span<const std::byte> data, std::span<std::byte> target) = 0;
        virtual void Receive(std::span<std::byte> target) = 0;
    };

}
}
}
