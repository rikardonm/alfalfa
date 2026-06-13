#include <alfalfa/lib/types/triplet-version.hpp>

#include <tuple>
#include <sstream>
#include <format>


namespace alfalfa
{
namespace lib
{

    std::optional<TripletVersion> TripletVersion::FromString(std::string& value)
    {
        try
        {
            std::istringstream iss(value);
            std::string major, minor, patch;
            std::getline(iss, major, '.');
            std::getline(iss, minor, '.');
            std::getline(iss, patch, '.');

            if (major.empty() || minor.empty() || patch.empty())
            {
                return std::nullopt;
            }

            return TripletVersion(std::stoi(major), std::stoi(minor), std::stoi(patch));
        }
        catch (const std::exception& ex)
        {
            return std::nullopt;
        }
    }

    bool TripletVersion::operator==(const TripletVersion& other) const
    {
        return std::tie(major, minor, patch) == std::tie(other.major, other.minor, other.patch);
    }

    bool TripletVersion::operator!=(const TripletVersion& other) const
    {
        return !(*this == other);
    }

    bool TripletVersion::operator<(const TripletVersion& other) const
    {
        return std::tie(major, minor, patch) < std::tie(other.major, other.minor, other.patch);
    }

    bool TripletVersion::operator>(const TripletVersion& other) const
    {
        return std::tie(major, minor, patch) > std::tie(other.major, other.minor, other.patch);
    }

    bool TripletVersion::operator<=(const TripletVersion& other) const
    {
        return std::tie(major, minor, patch) <= std::tie(other.major, other.minor, other.patch);
    }

    bool TripletVersion::operator>=(const TripletVersion& other) const
    {
        return std::tie(major, minor, patch) >= std::tie(other.major, other.minor, other.patch);
    }

    std::string ToString(const TripletVersion& version)
    {
        return std::format("{}.{}.{}", version.major, version.minor, version.patch);
    }

}
}
