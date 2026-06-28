#pragma once

#include <cstdint>
#include <optional>

#include <alfalfa/hal/two_wire.hpp>

using namespace Units::Literals;


namespace alfalfa
{
namespace bal
{

    class Aht10
    {
    public:
        Aht10(std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> module, bool address_selector);
        void Initialize();
        std::tuple<Units::Percent, Units::Celsius> ReadHumidityAndTemperature();
        void SoftReset(bool wait_for_completion = true);

    protected:
        std::byte WaitForCompletion(Units::Second timeout);

    private:
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _module;
        const std::byte _address;
    };

}
}
