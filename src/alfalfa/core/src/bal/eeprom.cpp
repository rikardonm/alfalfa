#include <alfalfa/bal/eeprom.hpp>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/oal/timekeeper.hpp>
#include <alfalfa/lib/errors.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{
namespace eeprom
{
    EepromI2cMemory::EepromI2cMemory(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> twire, const uint8_t dev_addr)
        : _module(twire)
        , _dev_addr(dev_addr)
    {
        assert(twire != nullptr);
    }

    std::vector<std::byte> EepromI2cMemory::Read(const uint16_t addr, std::size_t length)
    {
        std::vector<std::byte> data(length);
        ReadInto(addr, data);
        return data;
    }

    void EepromI2cMemory::ReadInto(const uint16_t addr, std::span<std::byte> target)
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto y = t::CollectionBuilder(
            t::StartWrite(_dev_addr),
            t::Write<1>({std::byte{static_cast<uint8_t>((addr & 0xFF) >> 0)}}),
            t::Write<1>({std::byte{static_cast<uint8_t>((addr & 0xFF) >> 8)}}),
            t::StartRead(_dev_addr),
            t::ReadInto(target)
        );
        _module->Execute(y);
    }

    void EepromI2cMemory::Write(const uint16_t addr, std::span<std::byte> data)
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto y = t::CollectionBuilder(
            t::StartWrite(_dev_addr),
            t::Write<1>({std::byte{static_cast<uint8_t>((addr & 0xFF) >> 0)}}),
            t::Write<1>({std::byte{static_cast<uint8_t>((addr & 0xFF) >> 8)}}),
            t::WriteFrom(data)
        );
        _module->Execute(y);

        // The 5ms value is given by the datasheet, twr max
        // https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8568-SEEPROM-AT24C256C-Datasheet.pdf
        // Second ref: https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24CS256-256-Kbit-3.4-MHz-I2C-Serial-EEPROM-with-128-Bit-Serial-Number-and-Enhanced-Software-Write-Protection-DS20005998.pdf
        // One could have used a backoff here; but, since we're counting loops with delays, meh, let's just round up.
        const auto timeout = 5_ms;
        const auto delay = 2_ms;
        const auto max_count = std::ceil(timeout / delay);
        for (auto poll_counter = 0; poll_counter < max_count; ++poll_counter)
        {
            try
            {
                namespace t = alfalfa::hal::two_wire::trx;
                auto y = t::CollectionBuilder(
                    t::StartWrite(_dev_addr)
                );
                _module->Execute(y);
                return;
            }
            catch (...)
            {
                // Handle the error
            }
            // Release the system for some time; this allows the CPU to do something else.
            Timekeeper::DumbDelay(delay);
        }
        throw alfalfa::Exception(alfalfa::ErrorCode::Timeout);
    }

    std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> EepromI2cMemory::GetBoundBus() const
    {
        return _module;
    }

}
}
}
