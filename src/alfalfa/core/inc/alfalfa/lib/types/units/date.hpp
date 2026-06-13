#pragma once

#include <cstdint>

namespace Units
{

    class Date
    {
        public:
            Date(uint32_t year, uint8_t month, uint8_t day)
                : _day(day), _month(month), _year(year)
                {}
        private:
            uint8_t _day;
            uint8_t _month;
            uint32_t _year;
    };

    namespace Literals
    {

    } /* namespace Literals */

} /* namespace Units */
