#include <alfalfa/bal/mcp4725.hpp>

#include <alfalfa/lib/types/units/all.hpp>

#include <alfalfa/oal/timekeeper.hpp>
#include <alfalfa/hal/dio.hpp>

#include <alfalfa/lib/log.hpp>
#include <alfalfa/lib/custom.hpp>

using namespace Units::Literals;

namespace Command
{
    #pragma pack(push, 1)
    static_assert(std::endian::native == std::endian::little);

    // Remember that these are byte-swapped! they shall be swapped before the transaction.
    struct WriteFast
    {
        uint16_t dac_data:12;
        bool pd0:1;
        bool pd1:1;
        // C0 and 1 are 0 for fast write
        const uint8_t _pad0:2 = 0b00;
    };
    static_assert(sizeof(WriteFast) == 2);

    struct WriteNormal
    {
        const uint8_t _pad0:4 = 0b0000;
        uint16_t dac_data:12;
        const bool _pad1:1 = 0b0;
        bool pd0:1;
        bool pd1:1;
        const uint8_t _pad2:2 = 0b00;
        bool c0:1;
        const bool _c1:1 = 0b1;
        const bool _c2:1 = 0b0;
    };
    static_assert(sizeof(WriteNormal) == 3);

    struct Read
    {
        uint16_t eeprom_dac_data:12;
        const uint8_t _pad4:1;
        bool eeprom_pd0:1;
        bool eeprom_pd1:1;
        const uint8_t _pad3:1;
        const uint8_t _pad2:4;
        uint16_t reg_dac_data:12;
        const uint8_t _pad1:1;
        bool reg_pd0:1;
        bool reg_pd1:1;
        const uint8_t _pad0:3;
        bool por:1;
        bool ready_not_busy:1;
    };
    static_assert(sizeof(Read) == 5);

    #pragma pack(pop)
}


namespace alfalfa
{
namespace bal
{

    MCP4725::MCP4725(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module, bool address_selector)
        : _module(std::move(module))
        , _address(address_selector ? std::byte{0b110'0001} : std::byte{0b110'0000})
    {
        Initialize();
    }

    void MCP4725::Initialize()
    {
        // No specific initialization sequence required.
    }

    void MCP4725::WriteOutput(const Units::Percent value, const bool fast_mode, const bool write_eeprom)
    {
        if (value < Units::percent_0 or value > Units::percent_100)
        {
            throw std::out_of_range("MCP4725: DAC percent value out of range (0-100%)");
        }
        const auto dac_data = (value / STEP_SIZE).DecayTo<uint16_t>(1, 1);
        WriteOutput(dac_data, fast_mode, write_eeprom);
    }

    void MCP4725::WriteOutput(const uint16_t value, const bool fast_mode, const bool write_eeprom)
    {
        if (value > MAX_COUNT)
        {
            throw std::out_of_range("MCP4725: DAC value out of range (0-4095)");
        }
        if (fast_mode and write_eeprom)
        {
            throw std::invalid_argument("MCP4725: Cannot use fast mode when writing to EEPROM");
        }
        namespace t = alfalfa::hal::two_wire::trx;
        if (fast_mode)
        {
            const auto cmd = Command::WriteFast{
                .dac_data = value,
                .pd0 = false,
                .pd1 = false
            };
            auto x = alfalfa::lib::AsBytes(cmd);
            std::array<std::byte, sizeof(cmd)> xx;
            std::copy(x.rbegin(), x.rend(), xx.begin());
            auto w = t::CollectionBuilder(
                t::StartWrite(_address),
                t::WriteFrom(xx)
            );
            _module->Execute(w);
        }
        else
        {
            const auto cmd = Command::WriteNormal{
                .dac_data = value,
                .pd0 = false,
                .pd1 = false,
                .c0 = write_eeprom
            };
            auto x = alfalfa::lib::AsBytes(cmd);
            std::array<std::byte, sizeof(cmd)> xx;
            std::copy(x.rbegin(), x.rend(), xx.begin());
            auto w = t::CollectionBuilder(
                t::StartWrite(_address),
                t::WriteFrom(xx)
            );
            _module->Execute(w);
        }
    }

    // void MCP4725::SetPowerMode(const bool p0, const bool pd1)
    // {
    //     // To set power mode, we need to read the current value and re-write it with new power bits.
    //     namespace t = alfalfa::hal::two_wire::trx;
    //     Command::Read reg_data{};
    //     const auto r = t::CollectionBuilder(
    //         t::StartRead(_address),
    //         t::ReadInto(alfalfa::lib::AsBytes(reg_data))
    //     );
    //     _module->Execute(r);

    //     const auto cmd = Command::WriteNormal{
    //         .dac_data = reg_data.reg_dac_data,
    //         .pd0 = p0,
    //         .pd1 = pd1,
    //         .c0 = false
    //     };
    //     auto w = t::CollectionBuilder(
    //         t::StartWrite(_address),
    //         t::WriteFrom(alfalfa::lib::AsBytes(cmd))
    //     );
    //     _module->Execute(w);
    // }

}
}
