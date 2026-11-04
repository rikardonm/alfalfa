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

    void Slave::Transmit(const std::byte data)
    {
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        std::ignore = __WriteByte(std::to_integer<uint8_t>(data));
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Transceive(const std::byte data, std::byte& target)
    {
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        target = static_cast<std::byte>(__WriteByte(std::to_integer<uint8_t>(data)));
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Receive(std::byte& target)
    {
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        target = static_cast<std::byte>(__WriteByte(0x00));
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Transmit(const std::span<const std::byte> data)
    {
        if (data.size() == 0)
        {
            return;
        }
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        for (auto& byte : data)
        {
            std::ignore = __WriteByte(std::to_integer<uint8_t>(byte));
        }
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Transceive(const std::span<const std::byte> data, std::span<std::byte> target)
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
        auto idx = std::size_t{0};
        for (auto& byte : data)
        {
            target[idx] = static_cast<std::byte>(__WriteByte(std::to_integer<uint8_t>(byte)));
            idx++;
        }
        _module->_clk->Write(false);
        _module->_mosi->Write(false);
        _chip_select_n->Write(true);
    }

    void Slave::Receive(std::span<std::byte> target)
    {
        if (target.size() == 0)
        {
            return;
        }
        _chip_select_n->Write(false);
        Timekeeper::Nop(1 * _nop_multiplier);
        for (auto& byte : target)
        {
            byte = static_cast<std::byte>(__WriteByte(0x00));
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
