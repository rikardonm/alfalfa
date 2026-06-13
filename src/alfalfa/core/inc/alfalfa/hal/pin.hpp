#pragma once

#include <cstdint>


namespace alfalfa
{
namespace hal
{
    /** @brief This class represents a physical pin of a device. */
    class Pin
    {
    public:
        constexpr Pin(const uint32_t io_index) : _index(io_index) {}

        /** @brief Get the pin number. */
        uint32_t IoIndex() const { return _index; }

    private:
        uint32_t _index;
    };
}
}
