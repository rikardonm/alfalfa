#pragma once

#include <cstdint>
#include <optional>
#include <tuple>

#include <alfalfa/hal/two_wire.hpp>
#include <alfalfa/lib/types/units/all.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{
namespace Bmp280
{
    enum class PowerMode : uint8_t
    {
        Sleep = 0b00,
        Forced = 0b01,
        Normal = 0b11,
    };

    enum class TemperatureSampling : uint8_t
    {
        Skip = 0b000,
        x1_16bit = 0b001,
        x2_17bit = 0b010,
        x4_18bit = 0b011,
        x8_19bit = 0b100,
        x16_20bit = 0b101,
    };

    enum class PressureSampling : uint8_t
    {
        Skip = 0b00,
        x1_UltraLowPower = 0b001,
        x2_LowPower = 0b010,
        x4_Normal = 0b011,
        x8_HighResolution = 0b100,
        x16_UltraHighResolution = 0b101,
    };

    class Bmp280
    {
    public:
        static const uint8_t expected_chip_id = 0x58;

        Bmp280(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module,
            bool address_selector = false);
        void Initialize();
        uint8_t ReadChipId(bool check = false) const;
        // Units::Celsius ReadTemperature() const;
        Units::Pascal ReadPressure() const;
        std::tuple<Units::Celsius, Units::Pascal> ReadCombo() const;
        void SetControlMeas(PowerMode mode, TemperatureSampling temp, PressureSampling pres) const;

    private:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _module;
        const std::byte _address;
        std::array<double, 3> _temperature_trim;
        std::array<double, 9> _pressure_trim;

        void ReadRegister(const std::byte start_addr, std::span<std::byte> target) const;
        void WriteRegister(const std::byte start_addr, const std::span<const std::byte> data) const;
        Units::Celsius CompensateTemperature(const int32_t value) const;
        Units::Pascal CompensatePressure(const int32_t value, const Units::Celsius temperature) const;
    };

}
}
}
