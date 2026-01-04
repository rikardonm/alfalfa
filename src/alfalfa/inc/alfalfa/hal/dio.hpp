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

    // Input digital I/O pin
    struct iInput
    {
        virtual ~iInput() = default;
        virtual void Configure(const InputConfiguration config) = 0;
        virtual bool Read() const = 0;
    };

    struct OutputConfiguration
    {
    };

    // Output digital I/O pin
    struct iOutput
    {
        virtual ~iOutput() = default;
        virtual void Configure(const OutputConfiguration config) = 0;
        virtual void Write(bool value) const = 0;
    };

}
}
}
