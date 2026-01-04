
#include <alfalfa/bal/mcp23017.hpp>
#include <alfalfa/oal/timekeeper.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{
namespace MCP23017
{

    // enum class RegisterMapBank1
    // {
    //     IODIRA   = 0x00,
    //     IPOLA    = 0x01,
    //     GPINTENA = 0x02,
    //     DEFVALA  = 0x03,
    //     INTCONA  = 0x04,
    //     IOCON    = 0x05,
    //     GPPUA    = 0x06,
    //     INTFA    = 0x07,
    //     INTCAPA  = 0x08,
    //     GPIOA    = 0x09,
    //     OLATA    = 0x0A,
    //     IODIRB   = 0x10,
    //     IPOLB    = 0x11,
    //     GPINTENB = 0x12,
    //     DEFVALB  = 0x13,
    //     INTCONB  = 0x14,
    //     GPPUB    = 0x16,
    //     INTFB    = 0x17,
    //     INTCAPB  = 0x18,
    //     GPIOB    = 0x19,
    //     OLATB    = 0x1A,
    // };

    Base::Base(
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> i2c_module,
        const bool a2,
        const bool a1,
        const bool a0,
        std::optional<std::shared_ptr<alfalfa::hal::dio::iOutput>> reset_n)
            : _wire(i2c_module)
            , _reset_n(reset_n)
            , _ax_address(static_cast<std::byte>(0x20 | (a0 << 0) | (a1 << 1) | (a2 << 2)))
            , _register_cache{}
            , _immediate_trx(true)
    {
        if (_reset_n.has_value())
        {
            _reset_n.value()->Write(false);
            Timekeeper::DumbDelay(10_us);
            _reset_n.value()->Write(true);
            Timekeeper::DumbDelay(10_us);
        }

        // We will ALWAYS (because we can) use BANK = 0
        ReInit();
    }

    void Base::ReInit()
    {
        // We will ALWAYS (because we can) use BANK = 0
        auto iocon = Registers::IOCON{.seqop = false, .bank = false};

        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<2>({static_cast<std::byte>(RegisterMap::IOCON), iocon.ToByte()})
        );
        _wire->Execute(x);

        auto y = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<1>({std::byte{0x00}}),
            t::StartRead(_ax_address),
            t::ReadInto(std::span{_register_cache})
        );
        _wire->Execute(y);
    }

    void Base::WriteRegister(const RegisterMap reg, const std::byte value)
    {
        _register_cache[static_cast<size_t>(reg)] = value;
        if (not _immediate_trx)
        {
            return;
        }

        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<2>({static_cast<std::byte>(reg), value})
        );
        _wire->Execute(x);
    }

    void Base::WriteRegisterMasked(const RegisterMap reg, const std::byte value, const std::byte mask)
    {
        auto current_value = ReadRegister(reg);
        auto new_value = (current_value & ~mask) | (value & mask);
        WriteRegister(reg, new_value);
    }

    std::byte Base::ReadRegister(const RegisterMap reg)
    {
        if (not _immediate_trx)
        {
            return _register_cache[static_cast<size_t>(reg)];
        }

        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<1>({static_cast<std::byte>(reg)}),
            t::StartRead(_ax_address),
            t::Read<1>()
        );
        _wire->Execute(x);
        auto r_value = std::get<3>(x.container)[0];
        _register_cache[static_cast<size_t>(reg)] = r_value;
        return r_value;
    }

    void Base::ReadSpan(const RegisterMap reg, std::span<std::byte> out_value)
    {
        if (not _immediate_trx)
        {
            std::copy_n(
                &_register_cache[static_cast<size_t>(reg)],
                out_value.size(),
                out_value.data());
            return;
        }

        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<1>({static_cast<std::byte>(reg)}),
            t::StartRead(_ax_address),
            t::ReadInto(out_value)
        );
        _wire->Execute(x);

        std::copy_n(
            out_value.data(),
            out_value.size(),
            &_register_cache[static_cast<size_t>(reg)]);
    }

    void Base::WriteSpan(const RegisterMap reg, const std::span<const std::byte> value)
    {
        if (not _immediate_trx)
        {
            std::copy_n(
                value.data(),
                value.size(),
                &_register_cache[static_cast<size_t>(reg)]);
            return;
        }

        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<1>({static_cast<std::byte>(reg)}),
            t::WriteFrom{value}
        );
        _wire->Execute(x);

        std::copy_n(
            value.data(),
            value.size(),
            &_register_cache[static_cast<size_t>(reg)]);
    }

    void Base::FlushCacheAndRead()
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<1>({std::byte{0x00}}),
            t::WriteFrom({std::span{_register_cache}}));
        _wire->Execute(x);

        auto y = t::CollectionBuilder(
            t::StartWrite(_ax_address),
            t::Write<1>({static_cast<std::byte>(RegisterMap::INTFA)}),
            t::StartRead(_ax_address),
            t::ReadInto(std::span{_register_cache} | std::views::drop(static_cast<size_t>(RegisterMap::INTFA)))
        );
        _wire->Execute(y);
    }

    UniformBusOutput::UniformBusOutput(
        std::shared_ptr<Base> base,
        const Port port)
            : port(port)
            , base(base)
    {
    }

    void UniformBusOutput::Configure(alfalfa::hal::dio::OutputConfiguration config)
    {
        base->WriteRegister(port == Port::A ? RegisterMap::IODIRA : RegisterMap::IODIRB, std::byte{0x00});
    }

    void UniformBusOutput::Write(const BusData &values)
    {
        uint8_t b_data = 0;
        // sooo.... we need to invert this fuck, because....
        // least significant goes to the last bit, doesn't it?
        for (auto& it : values)
        {
            b_data >>= 1;
            b_data |= it ? 0x80 : 0x00;
        }
        base->WriteRegister(port == Port::A ? RegisterMap::GPIOA : RegisterMap::GPIOB, std::byte{b_data});
    }

    OutputPin::OutputPin(
        std::shared_ptr<Base> base,
        const Port port,
        const uint8_t index)
            : _port(port)
            , _mask(std::byte{1} << index)
            , _base(base)
    {
    }

    void OutputPin::Configure(alfalfa::hal::dio::OutputConfiguration config)
    {
        _base->WriteRegisterMasked(
            _port == Port::A ? RegisterMap::IODIRA : RegisterMap::IODIRB,
            std::byte{0x00},
            _mask);
    }

    void OutputPin::Write(bool value) const
    {
        _base->WriteRegisterMasked(
            _port == Port::A ? RegisterMap::GPIOA : RegisterMap::GPIOB,
            value ? _mask : std::byte{0x00},
            _mask);
    }

    InputPin::InputPin(
        std::shared_ptr<Base> base,
        const Port port,
        const uint8_t index)
            : _port(port)
            , _mask(std::byte{1} << index)
            , _base(base)
    {
    }

    void InputPin::Configure(alfalfa::hal::dio::InputConfiguration config)
    {
        auto df = _base->DelayFlush();
        _base->WriteRegisterMasked(
            _port == Port::A ? RegisterMap::IODIRA : RegisterMap::IODIRB,
            _mask,
            _mask);
        _base->WriteRegisterMasked(
            _port == Port::A ? RegisterMap::GPPUA : RegisterMap::GPPUB,
            config.pull.has_value() && config.pull.value() ? _mask : std::byte{0x00},
            _mask
        );
    }

    bool InputPin::Read() const
    {
        auto reg_value = _base->ReadRegister(
            _port == Port::A ? RegisterMap::GPIOA : RegisterMap::GPIOB);
        return bool(reg_value & _mask);
    }

}
}
}
