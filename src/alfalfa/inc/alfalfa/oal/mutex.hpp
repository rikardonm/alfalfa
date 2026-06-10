#pragma once

#include <memory>
#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/impl/os/mutex.hpp>


namespace alfalfa
{
namespace oal
{
    /**
     * @class MutexLockRAII
     * @brief RAII wrapper that acquires a FreeRTOS mutex/semaphore in the constructor
     *        and releases it in the destructor if acquired.
     */
    class MutexLockRAII
    {
    public:
        /**
         * @brief Attempts to acquire the provided semaphore handle.
         * @param handle Reference to a FreeRTOS semaphore handle.
         * @param timeout Maximum time to wait for acquisition.
         */
        MutexLockRAII(MutexHandle& handle, const Units::Second timeout);

        // Disable copying
        MutexLockRAII(const MutexLockRAII&) = delete;
        MutexLockRAII& operator=(const MutexLockRAII&) = delete;

        // Enable moving - manual lifespan management
        MutexLockRAII(MutexLockRAII&&) = default;
        MutexLockRAII& operator=(MutexLockRAII&&) = default;

        /**
         * @brief Releases the semaphore if it was successfully acquired.
         */
        ~MutexLockRAII();

        /**
         * @brief Indicates whether the lock was successfully acquired.
         * @return true if the semaphore was acquired; otherwise false.
         */
        bool IsCaptured() const;

        /**
         * @brief Implicit conversion to acquisition state.
         * @return true if the semaphore was acquired; otherwise false.
         */
        operator bool() const;

    private:
        /** @brief Reference to the managed FreeRTOS semaphore handle. */
        MutexHandle& _handle;

        /** @brief True when the semaphore is currently held by this RAII object. */
        bool _captured;
    };

    /**
     * @class Mutex
     * @brief Thin wrapper around a FreeRTOS mutex handle.
     */
    class Mutex
    {
    public:
        /**
         * @brief Initializes the internal FreeRTOS mutex handle.
         */
        Mutex();
        ~Mutex();

        /**
         * @brief Attempts to lock the mutex for the specified timeout.
         * @param timeout Maximum time to wait for lock acquisition.
         * @return RAII lock object representing lock ownership.
         */
        MutexLockRAII Lock(const Units::Second timeout);
    private:
        /** @brief Underlying FreeRTOS mutex handle. */
        MutexHandle _mutex;
    };

    template<typename T>
    class LockedValueAccessor
    {
    public:
        LockedValueAccessor(T& value, MutexLockRAII&& lock_raii)
            : value(value)
            , _lock_raii(std::move(lock_raii))
        {
        }

        T& value;
    private:
        MutexLockRAII _lock_raii;
    };

    /**
     * @class LockedValue
     * @brief Thread-safe value wrapper protected by an internal mutex.
     * @tparam T Stored value type.
     */
    template<typename T>
    class LockedValue
    {
    public:
        /**
         * @brief Default-constructs the wrapped value.
         */
        LockedValue() = default;

        /**
         * @brief Constructs the wrapper with an initial value.
         * @param initial_value Initial value to store.
         */
        LockedValue(T initial_value)
            : _protected_value(initial_value)
        {
        }

        /**
         * @brief Attempts to acquire the mutex and access the protected value.
         * @param timeout Maximum time to wait for lock acquisition.
         * @return Optional accessor to the locked value.
         */
        std::optional<LockedValueAccessor<T>> Lock(const Units::Second timeout)
        {
            auto lock = _lock.Lock(timeout);
            if (not lock.IsCaptured())
            {
                return std::nullopt;
            }
            return {_protected_value, std::move(lock)};
        }

        /**
         * @brief Attempts to acquire the mutex and get a copy of the protected value.
         * @param timeout Maximum time to wait for lock acquisition.
         * @param default_value Value to return if lock acquisition fails.
         * @return Copy of the protected value if lock is acquired; otherwise default_value.
         */
        T GetCopy(const Units::Second timeout, T default_value)
        {
            auto lock = _lock.Lock(timeout);
            if (not lock.IsCaptured())
            {
                return default_value;
            }
            return _protected_value;
        }

        /**
         * @brief Attempts to acquire the mutex and set a new value.
         * @param timeout Maximum time to wait for lock acquisition.
         * @param new_value New value to set if lock is acquired.
         * @return true if the value was updated; otherwise false.
         */
        bool TrySet(const Units::Second timeout, const T& new_value)
        {
            auto lock = _lock.Lock(timeout);
            if (not lock.IsCaptured())
            {
                return false;
            }
            _protected_value = new_value;
            return true;
        }

    private:
        /** @brief Internal mutex protecting access to the value. */
        Mutex _lock;

        /** @brief The stored value. */
        T _protected_value;
    };

}
}
