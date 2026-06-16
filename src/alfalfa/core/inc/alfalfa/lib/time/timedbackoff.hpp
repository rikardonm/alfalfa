#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>


namespace alfalfa
{
namespace lib
{
namespace time
{

    /* Time-related activity helpers */

    /**
     * @brief Software time polling helper class.
     *
     */
    template<typename T>
    class TimedBackoff
    {
    public:
        /**
         * @brief Construct a new Timed Backoff object.
         *
         * @param wait_time The minimum time to wait before signalling expiration.
         */
        TimedBackoff(T wait_time, bool monotone = false)
        : _last_ts(0), _backoff(wait_time), _monotone(monotone)
        {
        }

        /**
         * @brief Check if protected action should be executed.
         *
         * @param now Current timestamp, in the same multiple/sub-multiple as the specified period.
         * @return true If action should be executed.
         * @return false If elapsed time is smaller than the specified period and action should not be executed.
         */
        bool Mark(const T now)
        {
            if (_monotone and (_last_ts > now))
            {
                _last_ts = now;
            }
            if (isExpired(now))
            {
                Remark(now);
                return true;
            }
            return false;
        }

        /**
         * @brief The same as `Mark`, but does not drift the last timestamp.
         *
         * @param now The current timestamp.
         * @return true If the action should be executed.
         * @return false If the action should not be executed.
         */
        bool MarkWithoutDrift(const T now)
        {
            bool acted = false;
            while (isExpired(now))
            {
                acted = true;
                _last_ts += _backoff;
            }
            return acted;
        }

        /**
         * @brief Check if the minimum period has elapsed.
         *
         * @param now The current timestamp.
         * @return True if the backoff time has elapsed, false otherwise.
         */
        bool isExpired(const T now)
        {
            return Elapsed(now) >= _backoff;
        }

        /**
         * @brief Get the elapsed time since the last mark.
         *
         * @param now
         * @return
         */
        T Elapsed(const T now)
        {
            return now - _last_ts;
        }

        /**
         * @brief Return the remaining time until expiration.
         *
         * @param future
         * @return
         */
        std::optional<T> Remaining(const T now)
        {
            if (isExpired(now))
            {
                return std::nullopt;
            }
            else
            {
                return _backoff - Elapsed(now);
            }
        }

        /**
         * @brief Update the timestamp of last expiration.
         *
         * @param now The current timestamp.
         */
        void Remark(const T now)
        {
            _last_ts = now;
        }

        /**
         * @brief Set a new value for the backoff.
         *
         * @param new_value The new value to use.
         */
        void SetBackoff(const T new_value)
        {
            _backoff = new_value;
        }

        /**
         * @brief Get the currently used backoff value.
         *
         * @return The value.
         */
        T GetBackoff() const
        {
            return _backoff;
        }

        /**
         * @brief Ensure that on the next `Mark` call the guard will allow execution.
         *
         */
        void EnsureMarkingOnNextHit()
        {
            _last_ts -= _backoff;
        }

        /**
         * @brief Delay the next hit by a specified amount of time.
         *
         * @param delay_by The amount of time to delay the next hit.
         */
        void PostponeHit(const T delay_by)
        {
            _last_ts += delay_by;
        }

    protected:
        T _last_ts;
        T _backoff;
        bool _monotone;
    };

}
}
}
