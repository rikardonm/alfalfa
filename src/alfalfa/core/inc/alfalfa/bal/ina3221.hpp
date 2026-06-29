#pragma once

#include <cstdint>
#include <optional>

#include <alfalfa/hal/two_wire.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{
namespace Ina3221
{

    enum class Channel
    {
        C1,
        C2,
        C3,
    };

    class Module
    {
    public:
        Module(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module,
            const alfalfa::hal::two_wire::AddressSelectorExtended address_selector);

        uint16_t ReadRegister(const std::optional<uint8_t> addr) const;
        void WriteRegister(const uint8_t addr, const uint16_t value) const;
        uint16_t ReadManufacturerId() const;
        uint16_t ReadDieId() const;
        Units::Volt ReadBusVoltage(const Channel channel) const;
        Units::Volt ReadShuntVoltage(const Channel channel) const;
        Units::Ampere ReadShuntCurrent(const Channel channel, const Units::Ohm shunt_resistance) const;
        static std::byte GetAddressForA0(const ::alfalfa::hal::two_wire::AddressSelectorExtended selector);

    private:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _module;
        const std::byte _address;
    };

}
}
}
