#pragma once

#include <cstdint>
#include <optional>

#include <alfalfa/hal/two_wire.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{

    /**
     * @brief This class represents an MCP4725 12-bit DAC device.
     *
     */
    class MCP4725
    {
    public:
        static const uint16_t MAX_COUNT = 0x0FFF;
        static constexpr Units::Percent STEP_SIZE = Units::percent_100 / MAX_COUNT;

        MCP4725(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module, bool address_selector);
        void Initialize();
        // Fast mode uses 2 bytes instead of 3 when transmitting data.
        // We can't use fast mode when writing to EEPROM.
        // In the future add contract pre-condition here.
        void WriteOutput(const uint16_t value, const bool fast_mode = true, const bool write_eeprom = false);
        void WriteOutput(const Units::Percent value, const bool fast_mode = true, const bool write_eeprom = false);
        // void SetPowerMode(const bool p0, const bool pd1);

        Units::Percent GetStep() const { return STEP_SIZE; }
        uint16_t GetMaxCount() const { return MAX_COUNT; }

    private:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _module;
        const std::byte _address;
    };

}
}
