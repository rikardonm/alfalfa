#include <alfalfa/hal/two_wire.hpp>


namespace alfalfa
{
namespace hal
{
namespace two_wire
{
namespace transactions
{
    _BuildContainer::_BuildContainer()
        : _collection()
    {
    }

    _BaseBuilder::_BaseBuilder()
        : _container()
    {
    }

    _BaseBuilder::_BaseBuilder(_BuildContainer&& builder)
        : _container(std::move(builder))
    {
    }

    WriteBuilder _BaseBuilder::RestartWrite(const std::optional<std::byte> address)
    {
        _container._collection.push_back(_StartCommand{});
        MakeAddress(address, false);
        return WriteBuilder(std::move(_container));
    }

    ReadBuilder _BaseBuilder::RestartRead(const std::optional<std::byte> address)
    {
        _container._collection.push_back(_StartCommand{});
        MakeAddress(address, true);
        return ReadBuilder(std::move(_container));
    }

    void _BaseBuilder::MakeAddress(const std::optional<std::byte> address, bool read_not_write)
    {
        std::byte addr;
        if (address)
        {
            addr = address.value();
            _container._last_address = addr;
        }
        else
        {
            addr = _container._last_address;
        }
        addr <<= 1;
        if (read_not_write)
        {
            addr |= std::byte{1};
        }
        _container._collection.push_back(_WriteCommand{.data = addr});
    }

    WriteBuilder& WriteBuilder::Send(const std::byte data)
    {
        _container._collection.push_back(_WriteCommand{.data = data});
        return *this;
    }

    WriteBuilder& WriteBuilder::Send(const std::span<const std::byte> data)
    {
        if (not data.empty())
        {
            _container._collection.push_back(_WriteCommand{.data = data});
        }
        return *this;
    }

    ReadBuilder& ReadBuilder::Into(std::span<std::byte> target)
    {
        if (not target.empty())
        {
            _container._collection.push_back(_ReadCommand{.data = target});
        }
        return *this;
    }

}
}
}
}
