#pragma once

#include <cstdint>
#include <array>
#include <tuple>
#include <ranges>
#include <exception>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/lib/time/timedbackoff.hpp>


namespace alfalfa
{
namespace lib
{
namespace time
{

    template<typename T>
    class TimeoutCheck
    {
    public:
        TimeoutCheck(const T timeout)
            : _backoff(timeout)
        {
        }

        void Remark(const T now)
        {
            _backoff.Remark(now);
        }

        void Check(const T now)
        {
            if (_backoff.Mark(now))
            {
                throw std::runtime_error("The operation has timed out.");
            }
        }

    private:
        alfalfa::lib::time::TimedBackoff<T> _backoff;
    };

}
}
}
