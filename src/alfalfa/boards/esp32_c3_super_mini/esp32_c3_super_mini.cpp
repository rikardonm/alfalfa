#include <alfalfa/boards/esp32_c3_super_mini.hpp>
#include <alfalfa/impl/hal/uart_cdc.hpp>


namespace alfalfa
{
namespace boards
{

    Esp32C3SuperMini::Esp32C3SuperMini()
        : blue_led(std::make_unique<alfalfa::hal::dio::Output>(
            alfalfa::hal::Pin(8), alfalfa::hal::dio::OutputConfiguration{}
        ))
        , uart(std::make_shared<alfalfa::hal::uart::CDCModule>())
    {
    }

    void Esp32C3SuperMini::SoftReset()
    {
        esp_restart();
    }

}
}
