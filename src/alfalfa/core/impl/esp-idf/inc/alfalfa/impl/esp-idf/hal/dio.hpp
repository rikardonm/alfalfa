#pragma once

#include <optional>

#include <alfalfa/hal/dio.hpp>


namespace alfalfa
{
namespace hal
{
namespace impl
{
namespace dio
{
    using namespace alfalfa::hal::dio;

    class Input : public iInput
    {
    public:
        Input(alfalfa::hal::Pin pin, const InputConfiguration config);
        void Configure(const InputConfiguration config);
        bool Read() const;

    private:
        const alfalfa::hal::Pin _pin;
    };

    class Output : public iOutput
    {
    public:
        Output(alfalfa::hal::Pin pin, const OutputConfiguration config);
        void Configure(const OutputConfiguration config) override;
        void Write(bool value) const override;

    private:
        const alfalfa::hal::Pin _pin;
    };

}
}
}
}
