#pragma once

#include <optional>
#include <alfalfa/lib/types/units/time.hpp>


namespace alfalfa
{
namespace lib
{
namespace time
{

    /* Time-related activity helpers */

    /**
     * @brief This class wraps the necessary information and logic for triggering scheduled events
     * in any time of the day.
     *
     * The class also takes into account a ratcheting system, where the `Mark` function could be called multiple times
     * with the same calendar value. Additionally, it is a **hard requirement** invoking the `Mark` function at least
     * once per second.
     */
    class ScheduledEvent
    {
    public:
        ScheduledEvent(Units::HourMinuteSecond schedule, std::optional<Units::TimeDuration> ratchet = {})
            : _in_ratchet(false)
            , _schedule(schedule)
            , _ratchet_schedule(schedule + ratchet.value_or(Units::TimeDuration{
                .microseconds=0, .seconds=1, .minutes=0, .hours=0, .days=0
            }))
        {
        }

        ScheduledEvent(Units::HourMinute schedule, std::optional<Units::TimeDuration> ratchet = {})
            : ScheduledEvent(Units::HourMinuteSecond(schedule.hour, schedule.minute, 0), ratchet)
        {
        }

        bool Mark(const ::tm now)
        {
            // test here the normal schedule
            if (not _in_ratchet)
            {
                if (_schedule == now)
                {
                    _in_ratchet = true;
                    return true;
                }
            }
            else if (_ratchet_schedule == now)
            {
                _in_ratchet = false;
            }
            return false;
        }

        void Reset()
        {
            _in_ratchet = false;
        }

    private:
        bool _in_ratchet;
        // Ideally both of these objects would be const;
        // BUT C++<20 doesn't allow for an object with const members to be moved or reassigned.
        Units::HourMinuteSecond _schedule;
        Units::HourMinuteSecond _ratchet_schedule;
    };

}
}
}
