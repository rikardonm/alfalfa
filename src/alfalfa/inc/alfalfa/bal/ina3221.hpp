#pragma once

#include <cstdint>
#include <optional>

#include <alfalfa/hal/two_wire.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{

    class Ina3221
    {
    public:
        Ina3221(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module,
            const alfalfa::hal::two_wire::AddressSelector address_selector);

        uint16_t ReadRegister(const std::optional<uint8_t> addr) const;
        void WriteRegister(const uint8_t addr, const uint16_t value) const;
        uint16_t ReadManufacturerId() const;
        uint16_t ReadDieId() const;
        Units::Volt ReadBusVoltage(const uint8_t channel) const;
        Units::Volt ReadShuntVoltage(const uint8_t channel) const;
        static std::byte GetAddressForA0(const ::alfalfa::hal::two_wire::AddressSelector selector);

    private:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _module;
        const std::byte _address;
    };

}
}
