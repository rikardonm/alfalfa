#pragma once

#include <alfalfa/hal/two_wire.hpp>

#include <driver/i2c_master.h>

using namespace Units::Literals;


namespace alfalfa
{
namespace hal
{
namespace two_wire
{
    class BusMaster : public iBusMaster
    {
    public:
        BusMaster(
            const i2c_port_t module_num,
            const ::alfalfa::hal::Pin scl,
            const ::alfalfa::hal::Pin sda,
            const Units::Hertz frequency = 400_kHz,
            const Units::Second timeout = 10_ms);

        virtual ~BusMaster();
        void Execute(transactions::_BuildContainer& builder) override;

    private:
        i2c_master_bus_handle_t bus_handle;
        i2c_master_dev_handle_t dev_handle;
        Units::Hertz bus_frequency;
        Units::Second transaction_timeout;
    };

}
}
}
