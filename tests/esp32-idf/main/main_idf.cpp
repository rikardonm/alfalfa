#include <cstddef>
#include <memory>
#include <filesystem>
#include <variant>
#include <span>

#include <alfalfa/app/abm.hpp>
#include <freertos/FreeRTOS.h>


extern "C" void app_main()
{
    while(true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
