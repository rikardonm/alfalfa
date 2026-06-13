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
     * @brief An interface for a simple SPI communication module.
     *
     */
    class iModule
    {
    public:
        virtual ~iModule() = default;
        virtual void Transceive(const uint8_t data) = 0;
        virtual void Transceive(const uint8_t data, uint8_t& target) = 0;
        virtual void Transceive(const std::span<const uint8_t> data) = 0;
        virtual void Transceive(const std::span<const uint8_t> data, std::span<uint8_t> target) = 0;
        virtual void Flush() = 0;
    };
}
}
}
