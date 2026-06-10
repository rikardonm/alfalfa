#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>


namespace alfalfa
{
namespace oal
{
namespace _impl
{

    class StaticTask
    {
    public:
        StaticTask()
            : task_buffer()
            , task_handle(nullptr)
        {
        }

        virtual ~StaticTask() = default;

    protected:
        virtual void Run() = 0;

        void _Start(
            const char* name,
            const std::size_t stack_size,
            uint8_t* stack_buffer,
            const std::optional<int> priority = std::nullopt,
            std::optional<int> core = std::nullopt);

        void RegisterWatchdog();
        void ResetWatchdog();
        void UnregisterWatchdog();

    private:
        StaticTask_t task_buffer;
        TaskHandle_t task_handle;

        static void Entrypoint(void* args);
    };

}
}
}
