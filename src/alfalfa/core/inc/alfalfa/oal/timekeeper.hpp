#pragma once

#include <string>
#include <ctime>
#include <cmath>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/lib/time/timedbackoff.hpp>
#include <alfalfa/lib/time/timeout.hpp>
#include <alfalfa/lib/log.hpp>

#include <alfalfa/impl/os/timekeeper.hpp>


namespace alfalfa
{
namespace oal
{
    class TickBackoff : public alfalfa::lib::time::TimedBackoff<TickType>
    {
    public:
        using basetype = alfalfa::lib::time::TimedBackoff<TickType>;
        using basetype::TimedBackoff;
        using basetype::Mark;
        using basetype::MarkWithoutDrift;
        using basetype::isExpired;
        using basetype::Remark;
        using basetype::SetBackoff;
        using basetype::GetBackoff;

        TickBackoff(const Units::Second interval);

        TickType Now();

        /**
         * @brief Check if the defined time has expired.
         *
         * @return true If yes.
         * @return false If elapsed time is smaller than the specified period.
         */
        bool Mark();

        bool MarkWithoutDrift();

        bool isExpired();

        void Remark();

        /**
         * @brief Delay (block) until the next mark.
         *
         */
        void DelayUntilMark();

        void SetBackoff(const Units::Second value);

        void PostponeHit(const Units::Second delay_by);

        static TickType ConvertToTicks(const Units::Second period);
        static TickType ConvertToTicks(const time_t period);
        static time_t ConvertToTimet(const TickType ticks);
    };

    class TickTimeout : public alfalfa::lib::time::TimeoutCheck<TickType>
    {
    public:
        using basetype = alfalfa::lib::time::TimeoutCheck<TickType>;
        using basetype::TimeoutCheck;
        using basetype::Check;
        using basetype::Remark;

        void Check();
        void Remark();

        TickTimeout(const Units::Second timeout);
    };

    struct TimeSource
    {
        /** The priority of the source used */
        uint8_t priority;
        std::string name;

        TimeSource(const uint8_t priority, const std::string name);
    };

    std::string ToString(TimeSource source);

    class Timekeeper
    {
    public:
        static const TimeSource default_time_source;

        Timekeeper();

        void Set(const TimeSource source);

        void Set(::tm new_ts, const TimeSource source);

        const TimeSource GetSource();

        void SetTimezone(std::string new_tz);

        /**
        * @brief Get the current timesamp.
        *
        * @return Seconds elapsed since the epoch.
        */
        static time_t Stamp();

        static TickType TickStamp();

        /**
         * @brief Create a backoff timer with a specified period.
         *
         * At initialization the current timestamp is captured.
         *
         * @param period The period to wait.
         * @return TickBackoff The backoff timer.
         */
        static TickBackoff MonotoneInterval(const Units::Second period);

        /**
         * @brief Create a timeout check object with the given timeout period.
         *
         * @param timeout
         * @return TickTimeout
         */
        static TickTimeout TimeoutCheck(const Units::Second timeout);

        /**
        * @brief Get the current date-timestamp.
        * The struct deviates from the specification(https://en.cppreference.com/w/c/chrono/tm) in the year value.
        * Whereas the spec. defines a difference from 1900, we use an absolute value.
        * @return A `::tm` structure with the year field adjusted.
        */
        static ::tm Now();

        /**
        * @brief Get the Day In Week, with Sunday = 0.
        *
        * @return An integer value representing the day in the week.
        */
        static uint8_t GetDayInWeek();

        /**
        * @brief Convert a timestamp (from Stamp) into a calendar date structure.
        *
        * @param timestamp The timestamp to convert.
        * @return A `::tm` structure with the year field adjusted.
        */
        static ::tm GetLocalCalendar(const time_t timestamp);

        /**
        * @brief Convert a calendar date structure into a timestamp value.
        *
        * @param cal_date The calendar date to convert.
        * @return A `::time_t` value.
        */
        static ::time_t GetStampFromCalendar(const ::tm cal_date);

        /**
        * @brief Execute a dumb action and cause a delay. Note: context switch may happen!
        *
        * @param time The desired time to stall.
        */
        static void DumbDelay(const Units::Second time);

        /**
         * @brief Execute a no-operation (NOP) instruction.
         *
         * @param count The number of NOP instructions to execute.
         */
        static void Nop(uint32_t count);

        /**
         * @brief Get the uptime of the system, in microseconds.
         *
         * @return uint64_t The uptime in microseconds.
         */
        static uint64_t GetUptime();

    private:
        TimeSource _used_source;
    };

}
}

using Timekeeper = alfalfa::oal::Timekeeper;
