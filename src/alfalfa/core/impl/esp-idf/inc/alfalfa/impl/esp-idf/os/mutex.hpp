#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>


namespace alfalfa
{
namespace oal
{
    using MutexHandle = SemaphoreHandle_t;
}
}
