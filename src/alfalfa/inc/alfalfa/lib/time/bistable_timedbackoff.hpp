#pragma once

#include <cstdint>
#include <cstddef>


namespace alfalfa
{
namespace lib
{
namespace time
{

    /**
     * @brief Software time polling helper class.
     *
     */
    template<typename T>
    class BistableTimedBackoff
    {
    public:
        BistableTimedBackoff(T on_time = 1, T off_time = 1, bool monotone = true)
        : _last_ts(0), _on_time(on_time), _off_time(off_time), _monotone(monotone), _last_state(false)
        {
        }

        /**
         * @brief Switches the state driven by the given times. Returns the output state.
         *
         * @param now
         * @return true
         * @return false
         */
        bool Mark(const T now)
        {
            if (_monotone and (_last_ts > now))
            {
                _last_ts = now;
            }
            if (not _on_time)
            {
                return false;
            }
            if (not _off_time)
            {
                return true;
            }
            auto time = _last_state ? _on_time : _off_time;
            if (isExpired(now, time))
            {
                Remark(now);
                // Toggle
                _last_state ^= true;
            }
            return _last_state;
        }

        /**
         * @brief Check if the minimum period has elapsed.
         *
         * @param now The current timestamp.
         * @return True if the backoff time has elapsed, false otherwise.
         */
        bool isExpired(const T now, const T backoff)
        {
            auto elapsed = now - _last_ts;
            return elapsed >= backoff;
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

        void SetTimes(T on_time, T off_time)
        {
            _on_time = on_time;
            _off_time = off_time;
        }

    private:
        T _last_ts;
        T _on_time;
        T _off_time;
        bool _monotone;
        bool _last_state;
    };

}
}
}
