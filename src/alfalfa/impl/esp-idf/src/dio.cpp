#include <alfalfa/impl/esp-idf/hal/dio.hpp>

#include "driver/gpio.h"


namespace alfalfa
{
namespace hal
{
namespace impl
{
namespace dio
{
    using namespace alfalfa::hal::dio;

    Input::Input(::alfalfa::hal::Pin pin, const InputConfiguration config)
        : _pin(pin)
    {
        Configure(config);
    }

    void Input::Configure(const InputConfiguration config)
    {
        auto esp_num = static_cast<gpio_num_t>(_pin.IoIndex());
        gpio_reset_pin(esp_num);
        gpio_set_direction(esp_num, GPIO_MODE_INPUT);
        if (config.pull.has_value())
        {
            gpio_set_pull_mode(esp_num, config.pull.value() ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY);
        }
        else
        {
            gpio_set_pull_mode(esp_num, GPIO_FLOATING);
        }
    }

    bool Input::Read() const
    {
        return gpio_get_level(static_cast<gpio_num_t>(_pin.IoIndex()));
    }

    Output::Output(::alfalfa::hal::Pin pin, const OutputConfiguration config)
        : _pin(pin)
    {
        Configure(config);
    }

    void Output::Configure(const OutputConfiguration config)
    {
        auto esp_num = static_cast<gpio_num_t>(_pin.IoIndex());
        gpio_reset_pin(esp_num);
        switch (config.type)
        {
            case OutputType::PushPull:
                gpio_set_direction(esp_num, GPIO_MODE_OUTPUT);
                break;
            case OutputType::OpenDrain:
                gpio_set_direction(esp_num, GPIO_MODE_OUTPUT_OD);
                break;
        }
        gpio_set_drive_capability(esp_num, GPIO_DRIVE_CAP_3);
    }

    void Output::Write(bool value) const
    {
        gpio_set_level(static_cast<gpio_num_t>(_pin.IoIndex()), value);
    }

}
}
}
}
