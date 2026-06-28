#include <memory>

#include <alfalfa/hal/uart.hpp>
#include <alfalfa/bal/led.hpp>
#include <alfalfa/impl/esp-idf/hal/dio.hpp>


namespace alfalfa
{
namespace boards
{

    struct Esp32C3SuperMini
    {
        alfalfa::bal::LED blue_led;
        std::shared_ptr<alfalfa::hal::uart::iModule> uart;

        Esp32C3SuperMini();

        void SoftReset();
    };

}
}
