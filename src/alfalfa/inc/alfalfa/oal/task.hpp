#pragma once

#include <cstdint>
#include <optional>

#include <alfalfa/lib/log.hpp>
#include <alfalfa/impl/os/task.hpp>


namespace alfalfa
{
namespace oal
{
    template<std::size_t STACK_SIZE>
    class StaticTask : public _impl::StaticTask
    {
    public:
        StaticTask()
            : _impl::StaticTask()
            , storage()
        {
        }
        virtual ~StaticTask() = default;

        /**
         * @brief Start the task. This will call the Run() method in a new thread of execution.
         *
         * @param name The name of the task, used for debugging purposes.
         * @param priority The priority of the task, higher values indicate higher priority. If not provided, defaults to IDLE + 1.
         * @param core The core on which the task should run. If not provided, the task will run on any available core.
         */
        void Start(const char* name, const std::optional<int> priority = std::nullopt, const std::optional<int> core = std::nullopt)
        {
            _impl::StaticTask::_Start(name, storage.size(), storage.begin(), priority, core);
        }

        using _impl::StaticTask::RegisterWatchdog;
        using _impl::StaticTask::ResetWatchdog;
        using _impl::StaticTask::UnregisterWatchdog;

    private:
        std::array<uint8_t, STACK_SIZE> storage;
    };
}
}
