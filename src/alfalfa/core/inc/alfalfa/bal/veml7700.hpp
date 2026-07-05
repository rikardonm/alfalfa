#pragma once

#include <cstdint>
#include <optional>
#include <tuple>
#include <ranges>

#include <alfalfa/hal/two_wire.hpp>
#include <alfalfa/lib/types/units/all.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{
namespace Veml7700
{
    enum class PowerSavingModes : uint8_t
    {
        Mode0 = 0b0000'0000,
        Mode1 = 0b0000'0010,
        Mode2 = 0b0000'0100,
        Mode3 = 0b0000'0110,
    };

    enum class AlsGain : uint16_t
    {
        // x1
        x1 = 0b0000'000'0000'0000,
        // x2
        x2 = 0b0000'100'0000'0000,
        // 1/8 gain
        x1_8 = 0b0001'000'0000'0000,
        // 1/4 gain
        x1_4 = 0b0001'100'0000'0000,
    };

    enum class AlsIntegrationTime : uint16_t
    {
        ms25  = 0b0000'0011'0000'0000,
        ms50  = 0b0000'0010'0000'0000,
        ms100 = 0b0000'0000'0000'0000,
        ms200 = 0b0000'0000'0100'0000,
        ms400 = 0b0000'0000'1000'0000,
        ms800 = 0b0000'0000'1100'0000,
    };

    enum class AlsPersistenceProtect : uint16_t
    {
        p1 = 0b0000'0000'0000'0000,
        p2 = 0b0000'0000'0001'0000,
        p4 = 0b0000'0000'0010'0000,
        p8 = 0b0000'0000'0011'0000,
    };

    class Veml7700
    {
    public:
        static constexpr std::array<uint16_t, 2> expected_chip_id = {0xC481, 0xD481};

        Veml7700(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module
        )
            : _module(module)
            , _address(std::byte{0x10})
        {
        }

        void SetConfiguration(
            AlsGain als_gain,
            std::optional<AlsIntegrationTime> als_int_time,
            AlsPersistenceProtect als_persistence,
            bool als_shutdown)
        {
            // bool als_integration_en,
            uint16_t value = 0x00;
            value |= std::to_underlying(als_gain);
            value |= std::to_underlying(als_persistence);
            if (als_int_time)
            {
                value |= std::to_underlying(als_int_time.value());
                value |= 0x02;
            }
            if (als_shutdown)
            {
                value |= 0x01;
            }
            WriteRegister(std::byte{0x00}, alfalfa::lib::AsBytes(value));
        }

        void SetPowerSavingMode(bool enable, PowerSavingModes mode = PowerSavingModes::Mode0)
        {
            auto buf = std::array{
                std::byte{std::to_underlying(mode)}
                | (enable ? std::byte{0x01} : std::byte{0x00}),
                std::byte{0x00}
            };
            WriteRegister(std::byte{0x03}, buf);
        }

        uint16_t ReadAlsData()
        {
            std::array<std::byte, 2> buff;
            ReadRegister(std::byte{0x04}, buff);
            const auto value = (std::to_integer<uint16_t>(buff[1]) << 8) | std::to_integer<uint16_t>(buff[0]);
            return value;
        }

        uint16_t ReadWhiteChannelData()
        {
            std::array<std::byte, 2> buff;
            ReadRegister(std::byte{0x05}, buff);
            const auto value = (std::to_integer<uint16_t>(buff[1]) << 8) | std::to_integer<uint16_t>(buff[0]);
            return value;
        }

        uint16_t ReadChipId(bool check = false) const
        {
            std::array<std::byte, 2> buff;
            ReadRegister(std::byte{0x07}, buff);
            const auto value = (std::to_integer<uint16_t>(buff[1]) << 8) | std::to_integer<uint16_t>(buff[0]);
            if (check and not std::ranges::contains(expected_chip_id, value))
            {
                throw std::runtime_error("BMP280: Invalid chip ID");
            }
            return value;
        }

    protected:
        void ReadRegister(const std::byte start_addr, std::span<std::byte> target) const
        {
            namespace t = alfalfa::hal::two_wire::trx;
            auto y = t::CollectionBuilder(
                t::StartWrite(_address),
                t::Write<1>({start_addr}),
                t::StartRead(_address),
                t::ReadInto(target)
            );
            _module->Execute(y);
        }

        void WriteRegister(const std::byte start_addr, const std::span<const std::byte> data) const
        {
            namespace t = alfalfa::hal::two_wire::trx;
            auto y = t::CollectionBuilder(
                t::StartWrite(_address),
                t::Write<1>({start_addr}),
                t::WriteFrom(data)
            );
            _module->Execute(y);
        }

    private:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _module;
        const std::byte _address;
    };

}
}
}
