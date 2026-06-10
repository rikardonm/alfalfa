#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <span>
#include <vector>

#include <alfalfa/hal/two_wire.hpp>
#include <alfalfa/lib/types/units/all.hpp>


namespace alfalfa
{
namespace bal
{
namespace eeprom
{
    class EepromI2cMemory
    {
    public:
        EepromI2cMemory(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module, const uint8_t dev_addr);
        std::vector<std::byte> Read(const uint16_t addr, std::size_t length);
        void ReadInto(const uint16_t addr, std::span<std::byte> target);
        void Write(const uint16_t addr, std::span<std::byte> data);
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> GetBoundBus() const;

    private:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _module;
        const uint8_t _dev_addr;
    };

    // AT24C256 chip
    class xT24C256 : public EepromI2cMemory
    {
    public:
        xT24C256(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> twire, const bool A2, const bool A1, const bool A0)
            : EepromI2cMemory(twire, 0x50 | (A2 << 2) | (A1 << 1) | A0)
        {
        }
    };
}
}
}
