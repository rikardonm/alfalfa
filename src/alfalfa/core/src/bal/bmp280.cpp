#include <utility>
#include <bit>

#include <alfalfa/bal/bmp280.hpp>

#include <alfalfa/lib/types/units/all.hpp>

#include <alfalfa/oal/timekeeper.hpp>
#include <alfalfa/hal/dio.hpp>

#include <alfalfa/lib/log.hpp>
#include <alfalfa/lib/custom.hpp>

using namespace Units::Literals;

// Adapted from: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf


// These registers start at address 0x88
struct CalibrationDataInMemory
{
    static const auto ADDRESS = std::byte{0x88};
    uint16_t t_1;
    int16_t t_2;
    int16_t t_3;
    uint16_t p_1;
    int16_t p_2;
    int16_t p_3;
    int16_t p_4;
    int16_t p_5;
    int16_t p_6;
    int16_t p_7;
    int16_t p_8;
    int16_t p_9;
};

struct MeasurementData
{
    static const auto ADDRESS = std::byte{0xF7};
    uint8_t p_msb;
    uint8_t p_lsb;
    uint8_t p_xlsb;
    uint8_t t_msb;
    uint8_t t_lsb;
    uint8_t t_xlsb;
};


namespace alfalfa
{
namespace bal
{
namespace Bmp280
{
    // Based off of: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf
    Bmp280::Bmp280(
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module,
        bool address_selector)
        : _module(std::move(module))
        , _address(address_selector ? std::byte{0x76} : std::byte{0x77})
    {
        Initialize();
    }
    void Bmp280::Initialize()
    {
        // get calibration data
        namespace t = alfalfa::hal::two_wire::trx;
        auto calib = CalibrationDataInMemory();
        auto y = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<1>({CalibrationDataInMemory::ADDRESS}),
            t::StartRead(_address),
            t::ReadInto(alfalfa::lib::AsBytes(calib))
        );
        _module->Execute(y);
        _temperature_trim[0] = calib.t_1;
        _temperature_trim[1] = calib.t_2;
        _temperature_trim[2] = calib.t_3;
        _pressure_trim[0] = calib.p_1;
        _pressure_trim[1] = calib.p_2;
        _pressure_trim[2] = calib.p_3;
        _pressure_trim[3] = calib.p_4;
        _pressure_trim[4] = calib.p_5;
        _pressure_trim[5] = calib.p_6;
        _pressure_trim[6] = calib.p_7;
        _pressure_trim[7] = calib.p_8;
        _pressure_trim[8] = calib.p_9;
    }

    uint8_t Bmp280::ReadChipId(bool check) const
    {
        std::array<std::byte, 1> buff;
        ReadRegister(std::byte{0xD0}, buff);
        const auto value = std::to_integer<uint8_t>(buff[0]);
        if (check and value != expected_chip_id)
        {
            throw std::runtime_error("BMP280: Invalid chip ID");
        }
        return value;
    }

    Units::Pascal Bmp280::ReadPressure() const
    {
        auto [temp, pres] = ReadCombo();
        return pres;
    }

    std::tuple<Units::Celsius, Units::Pascal> Bmp280::ReadCombo() const
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto m_data = MeasurementData();
        auto y = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<1>({MeasurementData::ADDRESS}),
            t::StartRead(_address),
            t::ReadInto(alfalfa::lib::AsBytes(m_data))
        );
        _module->Execute(y);
        uint32_t t_raw = (
            m_data.t_msb << 12
            | m_data.t_lsb << 4
            | (m_data.t_xlsb & 0xF0) >> 4
        );
        uint32_t p_raw = (
            m_data.p_msb << 12
            | m_data.p_lsb << 4
            | (m_data.p_xlsb & 0xF0) >> 4
        );
        const auto temp = CompensateTemperature(std::bit_cast<int32_t>(t_raw));
        const auto pres = CompensatePressure(std::bit_cast<int32_t>(p_raw), temp);
        return std::make_tuple(temp / 5120.0, pres);
    }

    void Bmp280::SetControlMeas(PowerMode mode, TemperatureSampling temp, PressureSampling pres) const
    {
        auto value = std::array{
            std::byte{std::to_underlying(mode)}
            | std::byte{std::to_underlying(pres)} << 2
            | std::byte{std::to_underlying(temp)} << 5
        };
        WriteRegister(std::byte{0xF4}, value);
    }

    void Bmp280::WriteRegister(const std::byte start_addr, const std::span<const std::byte> data) const
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto y = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<1>({start_addr}),
            t::WriteFrom(data)
        );
        _module->Execute(y);
    }

    void Bmp280::ReadRegister(const std::byte start_addr, std::span<std::byte> target) const
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

    Units::Celsius Bmp280::CompensateTemperature(const int32_t value) const
    {
        auto d_value = static_cast<double>(value);
        auto var1 = (d_value / 16384.0 - _temperature_trim[0] / 1024.0) * _temperature_trim[1];
        auto var2 = d_value / 131072.0 - _temperature_trim[0] / 8192.0;
        var2 = var2 * var2 * _temperature_trim[2];
        return Units::Celsius(var1 + var2);
    }

    Units::Pascal Bmp280::CompensatePressure(const int32_t value, const Units::Celsius temperature) const
    {
        auto var1 = (temperature.UnderlyingValue() / 2.0) - 64000.0;
        auto var2 = var1 * var1 * _pressure_trim[5] / 32768.0;
        var2 = var2 + var1 * _pressure_trim[4] * 2.0;
        var2 = (var2 / 4.0) + (_pressure_trim[3] * 65536.0);
        var1 = (_pressure_trim[2] * var1 * var1 / 524288.0 + _pressure_trim[1] * var1) / 524288.0;
        var1 = (1.0 + var1 / 32768.0) * _pressure_trim[0];
        if (var1 == 0.0)
        {
            return Units::Pascal(0); // avoid exception caused by division by zero
        }
        auto p = 1048576.0 - static_cast<double>(value);
        p = (p - (var2 / 4096.0)) * 6250.0 / var1;
        var1 = _pressure_trim[8] * p * p / 2147483648.0;
        var2 = p * _pressure_trim[7] / 32768.0;
        p = p + (var1 + var2 + _pressure_trim[6]) / 16.0;
        return Units::Pascal(p);
    }

}
}
}
