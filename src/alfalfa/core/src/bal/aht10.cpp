#include <alfalfa/bal/aht10.hpp>

#include <alfalfa/lib/types/units/all.hpp>

#include <alfalfa/oal/timekeeper.hpp>
#include <alfalfa/hal/dio.hpp>

#include <alfalfa/lib/log.hpp>
#include <alfalfa/lib/custom.hpp>

using namespace Units::Literals;


struct MeasurementData
{
    uint8_t state;
    uint8_t h_msb;
    uint8_t h_lsb;
    uint8_t h_t_combo;
    uint8_t t_msb;
    uint8_t t_lsb;
};


namespace alfalfa
{
namespace bal
{

    Aht10::Aht10(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module, bool address_selector)
        : _module(std::move(module))
        , _address(address_selector ? std::byte{0x38} : std::byte{0x39})
    {
        Initialize();
    }

    void Aht10::Initialize()
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<3>({std::byte{0xE1}, std::byte{0x08}, std::byte{0x00}})
        );
        _module->Execute(x);
    }

    std::tuple<Units::Percent, Units::Celsius> Aht10::ReadHumidityAndTemperature()
    {
        // first: trigger the measurement
        namespace t = alfalfa::hal::two_wire::trx;
        auto w = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<3>({std::byte{0xAC}, std::byte{0x33}, std::byte{0x00}})
        );
        _module->Execute(w);

        Timekeeper::DumbDelay(200_ms);

        auto m_data = MeasurementData();
        auto r = t::CollectionBuilder(
            t::StartRead(_address),
            t::ReadInto(alfalfa::lib::AsBytes(m_data))
        );
        _module->Execute(r);
        if (m_data.state & 0x80)
        {
            // Logln("BUSY!!");
        }
        auto m_h = Units::Percent(((m_data.h_t_combo & 0x0F << 16)) | (m_data.h_msb << 8) | m_data.h_lsb);
        auto m_t = Units::Celsius((m_data.t_msb << 12) | (m_data.t_lsb << 4) | (m_data.h_t_combo >> 4));
        return std::make_tuple(
            m_h / (1 << 20),
            ((m_t * 200) / (1 << 20)) - 50_C
        );
    }

    void Aht10::SoftReset()
    {
        // Soft reset implementation goes here
    }

}
}
