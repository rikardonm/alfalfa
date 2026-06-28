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

    auto GetHumidity()
    {
        auto m_h = Units::Percent(((h_t_combo & 0x0F << 16)) | (h_msb << 8) | h_lsb);
        return m_h / (1 << 20);
    }

    auto GetTemperature()
    {
        auto m_t = Units::Celsius((t_msb << 12) | (t_lsb << 4) | (h_t_combo >> 4));
        return ((m_t * 200) / (1 << 20)) - 50_C;
    }

};


namespace alfalfa
{
namespace bal
{

    namespace Aht10Commands
    {
        constexpr auto SoftReset = std::byte{0xBA};
        constexpr auto TriggerMeasurement = std::byte{0xAC};
        constexpr auto ReadStatus = std::byte{0x71};
        constexpr auto Initialize = std::byte{0xE1};
    };

    Aht10::Aht10(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module, bool address_selector)
        : _module(std::move(module))
        , _address(address_selector ? std::byte{0x38} : std::byte{0x39})
    {
        Initialize();
    }

    void Aht10::Initialize()
    {
        SoftReset(true);

        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<3>({Aht10Commands::Initialize, std::byte{0x08}, std::byte{0x00}})
        );
        _module->Execute(x);

        // auto status = WaitForCompletion(250_ms);

        // if ((status & std::byte{0x08}) == std::byte{0x00})
        // {
        //     throw std::runtime_error("AHT10: Initialization failed");
        // }
    }

    std::tuple<Units::Percent, Units::Celsius> Aht10::ReadHumidityAndTemperature()
    {
        // first: trigger the measurement
        namespace t = alfalfa::hal::two_wire::trx;
        auto w = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<3>({Aht10Commands::TriggerMeasurement, std::byte{0x33}, std::byte{0x00}})
        );
        _module->Execute(w);

        auto m_data = MeasurementData();
        auto r = t::CollectionBuilder(t::StartRead(_address), t::ReadInto(alfalfa::lib::AsBytes(m_data)));

        auto status = WaitForCompletion(75_ms);
        (void) status;

        _module->Execute(r);

        return std::make_tuple(m_data.GetHumidity(), m_data.GetTemperature());
    }

    void Aht10::SoftReset(bool wait_for_completion)
    {
        namespace t = alfalfa::hal::two_wire::trx;
        auto x = t::CollectionBuilder(
            t::StartWrite(_address),
            t::Write<1>({Aht10Commands::SoftReset})
        );
        _module->Execute(x);
        if (wait_for_completion)
        {
            Timekeeper::DumbDelay(20_ms);
        }
    }

    std::byte Aht10::WaitForCompletion(Units::Second timeout)
    {
        namespace t = alfalfa::hal::two_wire::trx;
        std::array<std::byte, 1> target;
        auto y = t::CollectionBuilder(t::StartRead(_address), t::ReadInto(target));

        auto timeout_tracker = Timekeeper::MonotoneInterval(timeout);
        while (true)
        {
            _module->Execute(y);
            if ((target[0] & std::byte{0x80}) == std::byte{0x00})
            {
                break;
            }
            if (timeout_tracker.Mark())
            {
                return std::byte{0xFF};
                // throw std::runtime_error("AHT10: Timeout waiting for measurement to complete");
            }
            Timekeeper::DumbDelay(30_ms);
        }

        return target[0];
    }


}
}
