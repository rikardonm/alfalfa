#pragma once

#include <cstdint>


namespace alfalfa
{
namespace lib
{

    struct Color
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;

        Color()
            : red(0)
            , green(0)
            , blue(0)
        {
        }

    };

}
}
