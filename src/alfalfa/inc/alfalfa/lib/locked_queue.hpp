#pragma once

#include <queue>
#include <alfalfa/oal/mutex.hpp>
#include <alfalfa/lib/types/units/all.hpp>


namespace alfalfa
{
namespace lib
{
    using namespace Units::Literals;

    template<typename TIN>
    struct LockedQueue
    {
        LockedQueue()
            : lock()
            , _data()
        {
        }

        bool Push(TIN message)
        {
            auto m = lock.Lock(10_ms);
            if (not m.IsCaptured())
            {
                return false;
            }
            _data.emplace_back(std::move(message));
            return true;
        }

        std::optional<TIN> Get()
        {
            auto m = lock.Lock(10_ms);
            if (not m.IsCaptured())
            {
                return std::nullopt;
            }
            if (_data.empty())
            {
                return std::nullopt;
            }
            auto msg = _data.front();
            _data.pop_front();
            return msg;
        }

        bool HasMessage()
        {
            auto m = lock.Lock(10_ms);
            if (not m.IsCaptured())
            {
                return false;
            }
            return not _data.empty();
        }

    private:
        alfalfa::oal::Mutex lock;
        std::deque<TIN> _data;
    };

}
}
