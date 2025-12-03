
#include <alfalfa/bal/mcp23017.hpp>
#include <alfalfa/oal/timekeeper.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{
namespace MCP23017
{

    enum class RegisterMapBank0
    {
        IODIRA = 0x00,
        IODIRB = 0x01,
        IPOLA = 0x02,
        IPOLB = 0x03,
        GPINTENA = 0x04,
        GPINTENB = 0x05,
        DEFVALA = 0x06,
        DEFVALB = 0x07,
        INTCONA = 0x08,
        INTCONB = 0x09,
        IOCON = 0x0A,
        GPPUA = 0x0C,
        GPPUB = 0x0D,
        INTFA = 0x0E,
        INTFB = 0x0F,
        INTCAPA = 0x10,
        INTCAPB = 0x11,
        GPIOA = 0x12,
        GPIOB = 0x13,
        OLATA = 0x14,
        OLATB = 0x15,
    };

    enum class RegisterMapBank1
    {
        IODIRA = 0x00,
        IODIRB = 0x01,
        IPOLA = 0x02,
        IPOLB = 0x03,
        GPINTENA = 0x04,
        GPINTENB = 0x05,
        DEFVALA = 0x06,
        DEFVALB = 0x07,
        INTCONA = 0x08,
        INTCONB = 0x09,
        IOCON = 0x0A,
        GPPUA = 0x0C,
        GPPUB = 0x0D,
        INTFA = 0x0E,
        INTFB = 0x0F,
        INTCAPA = 0x10,
        INTCAPB = 0x11,
        GPIOA = 0x12,
        GPIOB = 0x13,
        OLATA = 0x14,
        OLATB = 0x15,
    };

    _Base::_Base(
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> i2c_module,
        const bool a2,
        const bool a1,
        const bool a0,
        std::optional<std::shared_ptr<alfalfa::hal::dio::iOutput>> reset_n)
            : _wire(i2c_module)
            , _reset_n(reset_n)
            , _ax_address(static_cast<std::byte>(0x20 | (a0 << 0) | (a1 << 1) | (a2 << 2)))
    {
        if (_reset_n.has_value())
        {
            _reset_n.value()->Write(false);
            Timekeeper::DumbDelay(10_us);
            _reset_n.value()->Write(true);
            Timekeeper::DumbDelay(10_us);
        }

        // We will ALWAYS (because we can) use BANK = 0
        auto iocon = Registers::IOCON{.seqop = true, .bank = false};
        auto x = alfalfa::hal::two_wire::transactions::WriteBuilder::Start(_ax_address)
            .Send(static_cast<std::byte>(RegisterMapBank0::IOCON))
            .Send(std::byte{iocon.ToByte()});
        _wire->Execute(x);
    }

    void _Base::ReInit()
    {
        // We will ALWAYS (because we can) use BANK = 0
        auto x = alfalfa::hal::two_wire::transactions::WriteBuilder::Start(_ax_address)
            .Send(static_cast<std::byte>(RegisterMapBank0::IOCON))
            .Send(std::byte{0x00});
        _wire->Execute(x);
    }

    void UniformBusOutput::Configure(const Port port, alfalfa::hal::dio::OutputConfiguration config)
    {
        auto x = alfalfa::hal::two_wire::transactions::WriteBuilder::Start(_ax_address)
            .Send(static_cast<std::byte>(port == Port::A ? RegisterMapBank0::IODIRA : RegisterMapBank0::IODIRB))
            .Send(std::byte{0x00}); // All outputs
        _wire->Execute(x);
    }

    void UniformBusOutput::Write(const Port port, const BusData &values)
    {
        uint8_t b_data = 0;
        // sooo.... we need to invert this fuck, because....
        // least significant goes to the last bit, doesn't it?
        for (auto& it : values)
        {
            b_data >>= 1;
            b_data |= it ? 0x80 : 0x00;
        }
        auto x = alfalfa::hal::two_wire::transactions::WriteBuilder::Start(_ax_address)
            .Send(static_cast<std::byte>(port == Port::A ? RegisterMapBank0::GPIOA : RegisterMapBank0::GPIOB))
            .Send(std::byte{b_data});
        _wire->Execute(x);
    }

}
}
}
