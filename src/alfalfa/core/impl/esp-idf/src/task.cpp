#include <alfalfa/oal/task.hpp>
#include <alfalfa/impl/esp-idf/os/task.hpp>

#include <esp_task_wdt.h>


namespace alfalfa
{
namespace oal
{
namespace _impl
{

    void StaticTask::_Start(
        const char* name,
        const std::size_t stack_size,
        uint8_t* stack_buffer,
        const std::optional<int> priority,
        std::optional<int> core)
    {
        const auto prio = priority.value_or(tskIDLE_PRIORITY + 1);
        if (core.has_value())
        {
            this->task_handle = xTaskCreateStaticPinnedToCore(
                StaticTask::Entrypoint,
                name,
                stack_size,
                this,
                prio,
                stack_buffer,
                &this->task_buffer,
                core.value());
        }
        else
        {
            this->task_handle = xTaskCreateStatic(
                StaticTask::Entrypoint,
                name,
                stack_size,
                this,
                prio,
                stack_buffer,
                &this->task_buffer);
        }
    }

    void StaticTask::RegisterWatchdog()
    {
        esp_task_wdt_add(task_handle);
    }

    void StaticTask::ResetWatchdog()
    {
        esp_task_wdt_reset();
    }

    void StaticTask::UnregisterWatchdog()
    {
        esp_task_wdt_delete(task_handle);
    }

    void StaticTask::Entrypoint(void* args)
    {
        auto task = reinterpret_cast<StaticTask*>(args);
        try
        {
            task->Run();
        }
        catch (const std::exception& ex)
        {
            Logln("Exception: ", ex.what());
        }
        task->UnregisterWatchdog();
        vTaskDelete(nullptr);
    }

}
}
}