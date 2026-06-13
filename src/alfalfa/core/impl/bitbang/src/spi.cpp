#include <alfalfa/impl/bitbang/hal/spi.hpp>

#include <alfalfa/oal/timekeeper.hpp>


namespace alfalfa
{
namespace hal
{
namespace impl
{
namespace bitbang
{
namespace spi
{
    Module::Module(
        std::unique_ptr<::alfalfa::hal::dio::iInput>&& miso,
        std::unique_ptr<::alfalfa::hal::dio::iOutput>&& mosi,
        std::unique_ptr<::alfalfa::hal::dio::iOutput>&& clk,
        uint32_t nop_multiplier)
        : _miso(std::move(miso))
        , _mosi(std::move(mosi))
        , _clk(std::move(clk))
        , _nop_multiplier(nop_multiplier)
    {
    }

    void Module::Transceive(const uint8_t data)
    {
        Timekeeper::Nop(1 * _nop_multiplier);
        __WriteByte(data);
        _clk->Write(false);
        _mosi->Write(false);
    }

    void Module::Transceive(const uint8_t data, uint8_t& target)
    {
        Timekeeper::Nop(1 * _nop_multiplier);
        target = __WriteByte(data);
        _clk->Write(false);
        _mosi->Write(false);
    }

    void Module::Transceive(const std::span<const uint8_t> data)
    {
        if (data.size() == 0)
        {
            return;
        }
        Timekeeper::Nop(1 * _nop_multiplier);
        for (auto& byte : data)
        {
            std::ignore = __WriteByte(byte);
        }
        _clk->Write(false);
        _mosi->Write(false);
    }

    void Module::Transceive(const std::span<const uint8_t> data, std::span<uint8_t> target)
    {
        if (data.size() == 0)
        {
            return;
        }
        if (data.size() != target.size())
        {
            throw std::invalid_argument("Data and target spans must have the same size");
        }

        Timekeeper::Nop(1 * _nop_multiplier);
        auto idx = 0;
        for (auto& byte : data)
        {
            target[idx] = __WriteByte(byte);
            idx++;
        }
        _clk->Write(false);
        _mosi->Write(false);
    }

    void Module::Flush()
    {
        return;
    }

    uint8_t Module::__WriteByte(uint8_t byte)
    {
        uint8_t out = 0;
        for (int i = 0; i < 8; i++)
        {
            _clk->Write(false);
            _mosi->Write(byte & 0x80);
            byte <<= 1;
            Timekeeper::Nop(1 * _nop_multiplier);
            _clk->Write(true);
            Timekeeper::Nop(1 * _nop_multiplier);
            if (_miso)
            {
                out |= (_miso->Read() ? 0x01 : 0x00);
                out <<= 1;
            }
        }
        return out;
    }
}
}
}
}
}
