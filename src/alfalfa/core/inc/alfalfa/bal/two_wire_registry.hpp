#pragma once

#include <cstdint>
#include <vector>
#include <string>


namespace alfalfa
{
namespace bal
{

    //** Match the device address with known devices */
    std::vector<std::string> MatchDeviceAddress(uint8_t device_address);

}
}