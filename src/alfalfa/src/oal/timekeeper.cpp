#include <thread>
#include <chrono>
#include <string>
#include <ctime>
#include <cmath>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/lib/time/timedbackoff.hpp>
#include <alfalfa/lib/time/timeout.hpp>
#include <alfalfa/lib/log.hpp>

#include <alfalfa/oal/timekeeper.hpp>


namespace alfalfa
{
namespace oal
{

    TickType TickBackoff::Now()
    {
        return Timekeeper::TickStamp();
    }

    bool TickBackoff::Mark()
    {
        return basetype::Mark(Now());
    }

    bool TickBackoff::MarkWithoutDrift()
    {
        return basetype::MarkWithoutDrift(Now());
    }

    bool TickBackoff::isExpired()
    {
        return basetype::isExpired(Now());
    }

    void TickBackoff::Remark()
    {
        basetype::Remark(Now());
    }


    TickTimeout::TickTimeout(const Units::Second timeout)
        : alfalfa::lib::time::TimeoutCheck<TickType>(TickBackoff::ConvertToTicks(timeout))
    {
    }

    void TickTimeout::Check()
    {
        basetype::Check(Timekeeper::TickStamp());
    }

    void TickTimeout::Remark()
    {
        basetype::Remark(Timekeeper::TickStamp());
    }


    TimeSource::TimeSource(const uint8_t priority, const std::string name)
        : priority(priority), name(name)
    {
    }

    std::string ToString(TimeSource source)
    {
        return source.name;
    }


    Timekeeper::Timekeeper()
    : _used_source(default_time_source)
    {
    }

    TickTimeout Timekeeper::TimeoutCheck(const Units::Second timeout)
    {
        auto obj = TickTimeout(timeout);
        obj.Remark();
        return obj;
    }

    TickBackoff Timekeeper::MonotoneInterval(const Units::Second period)
    {
        // The base timer for the tick class is already monotone, so
        // the timedbackoff doesn't need to adjust for it.
        auto back = TickBackoff(period);
        back.Remark();
        return back;
    }

    ::tm Timekeeper::Now()
    {
        // get local time
        auto currentTime = Stamp();
        return GetLocalCalendar(currentTime);
    }

    uint8_t Timekeeper::GetDayInWeek()
    {
        return static_cast<uint8_t>(Now().tm_wday);
    }

    void Timekeeper::Set(const TimeSource source)
    {
        _used_source = source;
        auto now = Now();
        Logln("Time set by ", ToString(source), " to ", ToString(now));
    }

    const TimeSource Timekeeper::GetSource()
    {
        return _used_source;
    }

    const TimeSource Timekeeper::default_time_source(0, "Default");

}
}
