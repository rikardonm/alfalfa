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
    Slave::Slave(
        std::shared_ptr<Module> module,
        std::unique_ptr<::alfalfa::hal::dio::iOutput>&& chip_select_n,
        uint32_t nop_multiplier)
        : _module(module)
        , _chip_select_n(std::move(chip_select_n))
        , _nop_multiplier(nop_multiplier)
    {
        _chip_select_n->Write(true);
    }

    void Slave::Transceive(const uint8_t data)
    {
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        __WriteByte(data);
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Transceive(const uint8_t data, uint8_t& target)
    {
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        target = __WriteByte(data);
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Transceive(const std::span<const uint8_t> data)
    {
        if (data.size() == 0)
        {
            return;
        }
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        for (auto& byte : data)
        {
            std::ignore = __WriteByte(byte);
        }
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Transceive(const std::span<const uint8_t> data, std::span<uint8_t> target)
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
        _chip_select_n->Write(false);
        auto idx = 0;
        for (auto& byte : data)
        {
            target[idx] = __WriteByte(byte);
            idx++;
        }
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    uint8_t Slave::__WriteByte(uint8_t byte)
    {
        uint8_t out = 0;
        for (int i = 0; i < 8; i++)
        {
            _module->_clk->Write(false);
            _module->_mosi->Write(byte & 0x80);
            byte <<= 1;
            Timekeeper::Nop(1 * _nop_multiplier);
            _module->_clk->Write(true);
            Timekeeper::Nop(1 * _nop_multiplier);
            if (_module->_miso)
            {
                out |= (_module->_miso->Read() ? 0x01 : 0x00);
                out <<= 1;
            }
        }
        return out;
    }

    Module::Module(
        std::unique_ptr<::alfalfa::hal::dio::iInput>&& miso,
        std::unique_ptr<::alfalfa::hal::dio::iOutput>&& mosi,
        std::unique_ptr<::alfalfa::hal::dio::iOutput>&& clk)
        : _miso(std::move(miso))
        , _mosi(std::move(mosi))
        , _clk(std::move(clk))
    {
    }

}
}
}
}
}
