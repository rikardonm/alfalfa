#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/task.h>


namespace alfalfa
{
namespace oal
{
    // Use Freertos tick type
    using TickType = ::TickType_t;
}
}
