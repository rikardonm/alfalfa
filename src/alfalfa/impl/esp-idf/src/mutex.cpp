#include <alfalfa/impl/esp-idf/os/mutex.hpp>
#include <alfalfa/oal/mutex.hpp>


namespace alfalfa
{
namespace oal
{
    /**
     * @brief Attempts to acquire the provided semaphore handle.
     * @param handle Reference to a FreeRTOS semaphore handle.
     * @param timeout Maximum time to wait for acquisition.
     */
    MutexLockRAII::MutexLockRAII(MutexHandle& handle, const Units::Second timeout)
    : _handle(handle)
    {
        auto ticks = pdMS_TO_TICKS(timeout.To_time_t(std::milli()));
        _captured = xSemaphoreTake(_handle, ticks) == pdTRUE;
    }

    /**
     * @brief Releases the semaphore if it was successfully acquired.
     */
    MutexLockRAII::~MutexLockRAII()
    {
        if (_captured)
        {
            xSemaphoreGive(_handle);
        }
    }

    /**
     * @brief Indicates whether the lock was successfully acquired.
     * @return true if the semaphore was acquired; otherwise false.
     */
    bool MutexLockRAII::IsCaptured() const
    {
        return _captured;
    }

    /**
     * @brief Implicit conversion to acquisition state.
     * @return true if the semaphore was acquired; otherwise false.
     */
    MutexLockRAII::operator bool() const
    {
        return _captured;
    }


    /**
     * @brief Initializes the internal FreeRTOS mutex handle.
     */
    Mutex::Mutex()
    {
        _mutex = xSemaphoreCreateMutex();
    }

    Mutex::~Mutex()
    {
        if (_mutex != nullptr)
        {
            vSemaphoreDelete(_mutex);
        }
    }

    /**
     * @brief Attempts to lock the mutex for the specified timeout.
     * @param timeout Maximum time to wait for lock acquisition.
     * @return RAII lock object representing lock ownership.
     */
    MutexLockRAII Mutex::Lock(const Units::Second timeout)
    {
        return MutexLockRAII(_mutex, timeout);
    }

}
}
