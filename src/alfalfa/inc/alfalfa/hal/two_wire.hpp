#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <memory>
#include <exception>
#include <optional>
#include <variant>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/hal/pin.hpp>


namespace alfalfa
{
namespace hal
{
namespace two_wire
{
    class iBusMaster;

    enum class AddressSelector
    {
        GND,
        VCC,
        SDA,
        SCL,
    };

    class TwoWireException : public std::runtime_error
    {
        alfalfa::ErrorCode _error_code;
        uint32_t _lower_level_code = 0;
    public:
        TwoWireException(const std::string& message, alfalfa::ErrorCode error_code, uint32_t lower_level_code = 0)
            : std::runtime_error(message)
            , _error_code(error_code)
            , _lower_level_code(lower_level_code)
        {}
    };

    namespace transactions
    {
        class WriteBuilder;
        class ReadBuilder;

        struct _StartCommand
        {
        };

        struct _WriteCommand
        {
            // std::variant<const std::byte, const std::span<std::byte>, const std::array<const std::byte, 8>> data;
            std::variant<const std::byte, const std::span<const std::byte>> data;
        };
        struct _ReadCommand
        {
            std::span<std::byte> data;
        };

        using Collection = std::vector<std::variant<_StartCommand, _WriteCommand, _ReadCommand>>;

        struct _BuildContainer
        {
            friend class _BaseBuilder;
            _BuildContainer();

            Collection _collection;
            std::byte _last_address;
        };

        struct _BaseBuilder
        {
            friend class alfalfa::hal::two_wire::iBusMaster;

            _BaseBuilder();
            _BaseBuilder(_BuildContainer&& builder);
            WriteBuilder RestartWrite(std::optional<std::byte> address);
            ReadBuilder RestartRead(std::optional<std::byte> address);

            void MakeAddress(const std::optional<std::byte> address, bool read_not_write);

        protected:
            _BuildContainer _container;
        };

        struct WriteBuilder : public _BaseBuilder
        {
            static WriteBuilder Start(std::byte address)
            {
                return _BaseBuilder().RestartWrite(address);
            }
            WriteBuilder& Send(const std::byte data);
            WriteBuilder& Send(const std::span<const std::byte> data);
        };

        struct ReadBuilder : public _BaseBuilder
        {
            static ReadBuilder Start(std::byte address)
            {
                return _BaseBuilder().RestartRead(address);
            }
            ReadBuilder& Into(std::span<std::byte> target);
        };

        struct PreparedTransaction;
    }

    /**
     * @brief An interface for a simple, symmetrically-buffered Module communication module.
     *
     * The module provides an interface to build and subsequently execute structured "queries" or "transactions" on the
     * bus.
     *
     */
    struct iBusMaster
    {
        virtual ~iBusMaster() = default;

        virtual void Execute(transactions::_BuildContainer& builder) = 0;
        void Execute(transactions::_BaseBuilder& builder)
        {
            Execute(builder._container);
        }
    };

}
}
}
