#pragma once

#include <cstdint>
#include <cstddef>

#include <alfalfa/lib/time/timedbackoff.hpp>


namespace alfalfa
{
namespace lib
{
namespace time
{

    /**
     * @brief Wrapper for elapsed time software polling.
     *
     * This class provides a canvas for time-driven software-controlled actions.
     * By their control nature, the elapsed period of these tasks cannot be guaranteed, therefore only low priority
     * actions should use this class.
     *
     * The class is unaware of the time multiple used (s, ms, us, ...), as it is defined by the supplied period at
     * construction and the now timestamp information supplied in the poll (EventLoopStep).
     *
     */
    template<typename T>
    class PeriodicAction
    {
    public:
        /**
         * @brief Construct a new Periodic Action object.
         *
         * @param period Minimum action period.
         */
        PeriodicAction(const T period)
        : _backoff(period)
        {
        }

        /**
         * @brief Destroy the Periodic Action object.
         *
         */
        virtual ~PeriodicAction() = default;

        /**
         * @brief Initialize dependent blocks.
         *
         */
        virtual void Init()
        {}

        /**
         * @brief Verify if period has elapsed since previous execution and execute Action if so.
         *
         * This function should be called from the low-priority running loop, as the its duration is not defined.
         *
         * @param now The current timestamp.
         * @return If period has elapsed and the Action executed.
         */
        bool EventLoopStep(const T now)
        {
            if (not _backoff.Mark(now))
            {
                return false;
            }
            Action();
            return true;
        }
    protected:
        virtual void Action() = 0;
        alfalfa::lib::time::TimedBackoff<T> _backoff;
    };

}
}
}
