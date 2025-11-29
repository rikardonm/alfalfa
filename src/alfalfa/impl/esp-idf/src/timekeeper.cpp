#include <string>
#include <ctime>
#include <cmath>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/lib/time/timedbackoff.hpp>
#include <alfalfa/lib/log.hpp>

#include <alfalfa/oal/timekeeper.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/task.h>

#include <esp_timer.h>


namespace alfalfa
{
namespace oal
{

    TickBackoff::TickBackoff(const Units::Second interval)
        : basetype(pdMS_TO_TICKS(interval.To_time_t(std::milli())), false)
    {
    }

    void TickBackoff::DelayUntilMark()
    {
        vTaskDelayUntil(&_last_ts, _backoff);
    }

    void TickBackoff::SetBackoff(const Units::Second value)
    {
        basetype::SetBackoff(pdMS_TO_TICKS(value.To_time_t(std::milli())));
    }

    void Timekeeper::DumbDelay(const Units::Second time)
    {
        vTaskDelay(pdMS_TO_TICKS(time.DecayTo<uint32_t>(1000, 1)));
    }

    void Timekeeper::Nop(uint32_t count)
    {
        while(count--)
        {
            __asm__ ("nop");
        }
    }

    TickType Timekeeper::TickStamp()
    {
        return xTaskGetTickCount();
    }

    uint64_t Timekeeper::GetUptime()
    {
        return static_cast<uint64_t>(esp_timer_get_time());
    }

    TickType TickBackoff::ConvertToTicks(const Units::Second period)
    {
        return pdMS_TO_TICKS(period.To_time_t(std::milli()));
    }

    TickType TickBackoff::ConvertToTicks(const time_t period)
    {
        return pdMS_TO_TICKS(period * 1'000);
    }

    time_t TickBackoff::ConvertToTimet(const TickType_t ticks)
    {
        return pdTICKS_TO_MS(ticks) / 1'000;
    }

    void Timekeeper::Set(::tm new_ts, const TimeSource source)
    {
        // Only update if we are using a better source
        if (source.priority < _used_source.priority)
        {
            return;
        }
        // FOR WHATEVER FUCKING REASON THIS SHIT NEEDS TO BE HERE
        // although I do think it may have to do with the fucked up TZ format used.
        // https://forum.arduino.cc/t/time-library-functions-with-esp32-core/515397/21
        new_ts.tm_isdst = -1;
        auto new_time = mktime(&new_ts);
        auto new_timeval = ::timeval();
        new_timeval.tv_sec = new_time;
        new_timeval.tv_usec = 0;
        ::timezone utc = {0, 0};
        settimeofday(&new_timeval, &utc);
        _used_source = source;
        auto now = Now();
        Logln("Time set by ", ToString(source), " to ", ToString(now));
    }

    void Timekeeper::SetTimezone(std::string new_tz)
    {
        setenv("TZ", new_tz.c_str(), 1);
        tzset();
        Logln("Timezone set to ", new_tz);
    }

    time_t Timekeeper::Stamp()
    {
        return std::time(nullptr);
    }

    ::tm Timekeeper::GetLocalCalendar(const time_t timestamp)
    {
        struct tm lt;
        localtime_r(&timestamp, &lt);
        // increment 1900 as it follows the standard... and we don't.
        // https://en.cppreference.com/w/c/chrono/tm
        lt.tm_year += 1900;
        return lt;
    }

    ::time_t Timekeeper::GetStampFromCalendar(const ::tm cal_date)
    {
        auto date = cal_date;
        date.tm_year -= 1900;
        auto ret = mktime(&date);
        return ret;
    }

}
}
