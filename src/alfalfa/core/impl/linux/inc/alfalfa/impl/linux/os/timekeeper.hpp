#pragma once

#include <cstdint>


namespace alfalfa
{
namespace oal
{
    // For linux systems use a full-width 64-bit number
    using TickType = ::uint64_t;
    // And use a 1-1 conversion between ticks and ms
}
}
