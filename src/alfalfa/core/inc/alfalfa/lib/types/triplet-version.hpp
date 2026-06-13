#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <optional>
#include <exception>


namespace alfalfa
{
namespace lib
{

    /**
     * @brief This struct represents a version object with the format "M.m.p"
     *
     */
    struct TripletVersion
    {
        uint32_t major;
        uint32_t minor;
        uint32_t patch;

        TripletVersion(uint32_t major, uint32_t minor, uint32_t patch)
            : major(major), minor(minor), patch(patch)
        {}

        std::optional<TripletVersion> static FromString(std::string& value);

        bool operator==(const TripletVersion& other) const;
        bool operator!=(const TripletVersion& other) const;
        bool operator<(const TripletVersion& other) const;
        bool operator>(const TripletVersion& other) const;
        bool operator<=(const TripletVersion& other) const;
        bool operator>=(const TripletVersion& other) const;
    };

    std::string ToString(const TripletVersion& version);

}
}
