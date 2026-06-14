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
        virtual void Transceive(const uint8_t data) = 0;
        virtual void Transceive(const uint8_t data, uint8_t& target) = 0;
        virtual void Transceive(const std::span<const uint8_t> data) = 0;
        virtual void Transceive(const std::span<const uint8_t> data, std::span<uint8_t> target) = 0;
    };

}
}
}
