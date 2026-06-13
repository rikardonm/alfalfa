#include <alfalfa/bal/led.hpp>


namespace alfalfa
{
namespace bal
{
    LED::LED(std::unique_ptr<alfalfa::hal::dio::iOutput>&& out, bool low_side_load)
        : _output(std::move(out))
        , _last(false)
        , _low_side(low_side_load)
    {
    }

    void LED::Set(bool state)
    {
        if (state == _last)
        {
            return;
        }
        _output->Write(state != _low_side);
        _last = state;
    }

    void LED::On()
    {
        Set(true);
    }

    void LED::Off()
    {
        Set(false);
    }

    void LED::Toggle()
    {
        Set(!_last);
    }

    alfalfa::hal::dio::iOutput& LED::Output() const
    {
        return *_output;
    }

}
}
