#include <alfalfa/bal/ina3221.hpp>

#include <alfalfa/lib/types/units/all.hpp>

#include <alfalfa/oal/timekeeper.hpp>
#include <alfalfa/hal/dio.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{

    Ina3221::Ina3221(
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module,
        const alfalfa::hal::two_wire::AddressSelector address_selector)
        : _module(std::move(module))
        , _address(GetAddressForA0(address_selector))
    {
    }

    uint16_t Ina3221::ReadRegister(const std::optional<uint8_t> addr) const
    {
        namespace t = alfalfa::hal::two_wire::trx;
        std::array<std::byte, 2> buff;
        if (addr.has_value())
        {
            auto x = t::CollectionBuilder(
                t::StartWrite(_address),
                t::Write<1>({std::byte{addr.value()}}),
                t::StartRead(_address),
                t::Read<2>()
            );
            _module->Execute(x);
            buff = std::get<3>(x.container);
        }
        else
        {
            auto x = t::CollectionBuilder(
                t::StartRead(_address),
                t::Read<2>()
            );
            _module->Execute(x);
            buff = std::get<1>(x.container);
        }
        return std::to_integer<uint16_t>(buff[0]) << 8 | std::to_integer<uint16_t>(buff[1]);
    }

    void Ina3221::WriteRegister(const uint8_t addr, const uint16_t value) const
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<3>({
            std::byte{static_cast<uint8_t>(addr)},
            std::byte{static_cast<uint8_t>(value >> 8)},
            std::byte{static_cast<uint8_t>(value & 0xFF)}
        }));
        _module->Execute(x);
    }

    uint16_t Ina3221::ReadManufacturerId() const
    {
        return ReadRegister(0xFE);
    }

    uint16_t Ina3221::ReadDieId() const
    {
        return ReadRegister(0xFF);
    }

    Units::Volt Ina3221::ReadBusVoltage(const uint8_t channel) const
    {
        uint8_t addr = 0;
        switch (channel)
        {
            case 1:
                addr = 0x02;
                break;
            case 2:
                addr = 0x04;
                break;
            case 3:
                addr = 0x06;
                break;
            default:
                throw std::invalid_argument("Invalid INA3221 Bus Voltage channel selection");
        }
        const auto reg_value = static_cast<int16_t>(ReadRegister(addr)) >> 3;
        // Each bit corresponds to 8mV, resulting a total of 32.76V full-scale range.
        return 8.0_mV * reg_value;
    }

    Units::Volt Ina3221::ReadShuntVoltage(const uint8_t channel) const
    {
        uint8_t addr = 0;
        switch (channel)
        {
            case 1:
                addr = 0x01;
                break;
            case 2:
                addr = 0x03;
                break;
            case 3:
                addr = 0x05;
                break;
            default:
                throw std::invalid_argument("Invalid INA3221 Shunt Voltage channel selection");
        }
        const auto reg_value = static_cast<int16_t>(ReadRegister(addr)) >> 3;
        // Each bit corresponds to 40uV, resulting a total of 163.8mV full-scale range.
        return 40.0_uV * reg_value;
    }

    std::byte Ina3221::GetAddressForA0(const ::alfalfa::hal::two_wire::AddressSelector selector)
    {
        const std::byte value{0x40};
        switch (selector)
        {
            case ::alfalfa::hal::two_wire::AddressSelector::GND:
                return value | std::byte{0b00};
            case ::alfalfa::hal::two_wire::AddressSelector::VCC:
                return value | std::byte{0b01};
            case ::alfalfa::hal::two_wire::AddressSelector::SDA:
                return value | std::byte{0b10};
            case ::alfalfa::hal::two_wire::AddressSelector::SCL:
                return value | std::byte{0b11};
            default:
                throw std::invalid_argument("Invalid TwoWire::AddressSelector value");
        }
    }

}
}
