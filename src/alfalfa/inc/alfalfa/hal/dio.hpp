#pragma once

#include <optional>

#include <alfalfa/hal/pin.hpp>


namespace alfalfa
{
namespace hal
{
namespace dio
{

    struct InputConfiguration
    {
        std::optional<bool> pull;
    };

    struct iInput
    {
        virtual void Configure(const InputConfiguration config) = 0;
        virtual bool Read() const = 0;
    };

    struct OutputConfiguration
    {
    };

    struct iOutput
    {
        virtual void Configure(const OutputConfiguration config) = 0;
        virtual void Write(bool value) const = 0;
    };

}
}
}
