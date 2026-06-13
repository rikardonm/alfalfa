#include <alfalfa/bal/ads1x1x.hpp>
#include <exception>
#include <bit>


namespace alfalfa
{
namespace bal
{
namespace ADS1x1x
{

    std::byte GetAddress(alfalfa::hal::two_wire::AddressSelector addr)
    {
        switch (addr)
        {
            case alfalfa::hal::two_wire::AddressSelector::GND:  return std::byte{0x48};
            case alfalfa::hal::two_wire::AddressSelector::VCC:  return std::byte{0x49};
            case alfalfa::hal::two_wire::AddressSelector::SDA:  return std::byte{0x4A};
            case alfalfa::hal::two_wire::AddressSelector::SCL:  return std::byte{0x4B};
            default: throw std::invalid_argument("Invalid TwoWire::AddressSelector value");
        }
    }

    BaseModule::BaseModule(
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> two_wire_module,
        alfalfa::hal::two_wire::AddressSelector address)
        : _address(GetAddress(address))
        , _tw(two_wire_module)
    {
        if (not IsConnected())
        {
            throw std::runtime_error("ADS1x1x device not found at specified address");
        }
    }

    bool BaseModule::IsConnected()
    {
        auto x = alfalfa::hal::two_wire::trx::CollectionBuilder(
            alfalfa::hal::two_wire::trx::StartWrite(_address)
        );
        try
        {
            _tw->Execute(x);
            return true;
        }
        catch (const alfalfa::hal::two_wire::TwoWireException&)
        {
            return false;
        }
    }

    uint16_t BaseModule::ReadRegister(ADS1x1x::Register reg)
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<1>({std::byte{static_cast<uint8_t>(reg)}}),
            t::StartRead(_address),
            t::Read<2>()
        );
        _tw->Execute(x);
        const auto buff = std::get<3>(x.container);
        return (std::to_integer<uint16_t>(buff[0]) << 8) | std::to_integer<uint16_t>(buff[1]);
    }

    void BaseModule::WriteRegister(ADS1x1x::Register reg, uint16_t value)
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<3>({
            std::byte{static_cast<uint8_t>(reg)},
            std::byte{static_cast<uint8_t>(value >> 8)},
            std::byte{static_cast<uint8_t>(value & 0xFF)}
        }));
        _tw->Execute(x);
    }

    ADS1015::ADS1015(
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> two_wire_module,
        alfalfa::hal::two_wire::AddressSelector address)
        : ADS1x1x::BaseModule(two_wire_module, address)
        , _value_per_count(0.0)
    {
    }

    int16_t ADS1015::GetConversionResult()
    {
        // The ADS1015 left-justifies the 12-bit result in the 16-bit register.
        const auto result = ReadRegister(ADS1x1x::Register::Conversion);
        return std::bit_cast<int16_t>(result) >> 4;
    }

    Units::Volt ADS1015::GetVoltage()
    {
        return Units::Volt(_value_per_count * GetConversionResult());
    }

    void ADS1015::ConfigureSingleShot(
        const ConfigRegister::InputMuxConfig input,
        const ConfigRegister::GainConfig gain,
        const ConfigRegister::DataRate rate,
        const bool trigger)
    {
        auto reg = ConfigRegister{};
        reg.comp_queue = ConfigRegister::ComparatorQueue::DISABLE_COMPARATOR;
        reg.comparator_latch = false;
        reg.comparator_polarity = false;
        reg.comparator_mode = false;
        reg.data_rate = rate;
        reg.mode = true; // Single-shot
        reg.pga_gain = gain;
        reg.input_mux = input;
        reg.os_single_shot = trigger;
        WriteRegister(ADS1x1x::Register::Config, std::bit_cast<uint16_t>(reg));
        _SetGain(gain);
    }

    void ADS1015::ConfigureContinuosSampling(
        const ConfigRegister::InputMuxConfig input,
        const ConfigRegister::GainConfig gain,
        const ConfigRegister::DataRate rate)
    {
        auto reg = ConfigRegister{};
        reg.comp_queue = ConfigRegister::ComparatorQueue::DISABLE_COMPARATOR;
        reg.comparator_latch = false;
        reg.comparator_polarity = false;
        reg.comparator_mode = false;
        reg.data_rate = rate;
        reg.mode = false; // Continuous
        reg.pga_gain = gain;
        reg.input_mux = input;
        reg.os_single_shot = false;
        WriteRegister(ADS1x1x::Register::Config, std::bit_cast<uint16_t>(reg));
        _SetGain(gain);
    }

    void ADS1015::_SetGain(const ConfigRegister::GainConfig gain)
    {
        double fsr = 0;
        switch (gain)
        {
        case ConfigRegister::GainConfig::FSR_0256:
            fsr = 0.0256;
            break;
        case ConfigRegister::GainConfig::FSR_0512:
            fsr = 0.512;
            break;
        case ConfigRegister::GainConfig::FSR_1024:
            fsr = 1.024;
            break;
        case ConfigRegister::GainConfig::FSR_2048:
            fsr = 2.048;
            break;
        case ConfigRegister::GainConfig::FSR_4096:
            fsr = 4.096;
            break;
        case ConfigRegister::GainConfig::FSR_6144:
            fsr = 6.144;
            break;
        default:
            throw std::invalid_argument("Invalid GainConfig value");
        }
        // The ADS1015 has a 12-bit converter
        // in the datasheet fsr is defined as +/- full scale
        _value_per_count = fsr / ((1 << 11) - 1);
    }

}
}
}
