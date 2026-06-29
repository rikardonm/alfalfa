#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <memory>
#include <exception>
#include <optional>
#include <variant>
#include <type_traits>
#include <vector>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/hal/pin.hpp>

namespace alfalfa
{
    namespace hal
    {
        namespace two_wire
        {
            using namespace Units::Literals;

            class iBusMaster;

            enum class AddressSelector
            {
                GND,
                VCC,
            };

            enum class AddressSelectorExtended
            {
                GND,
                VCC,
                SDA,
                SCL,
            };

            using AddressVariant = std::variant<AddressSelector, AddressSelectorExtended>;

            class TwoWireException : public std::runtime_error
            {
                alfalfa::ErrorCode _error_code;
                uint32_t _lower_level_code = 0;

            public:
                TwoWireException(const std::string &message, alfalfa::ErrorCode error_code, uint32_t lower_level_code = 0)
                    : std::runtime_error(message), _error_code(error_code), _lower_level_code(lower_level_code)
                {
                }
            };

            namespace trx
            {
                struct Start
                {
                    // This is the address value already shifted
                    std::byte address_command;
                    // Whether or not to require an ACK from the slave device after sending the address
                    bool require_ack = false;
                };

                inline Start StartNone(std::byte address, bool require_ack = false)
                {
                    return {address << 1, require_ack};
                }

                inline Start StartNone(uint8_t address, bool require_ack = false)
                {
                    return StartNone(std::byte{address}, require_ack);
                }

                inline Start StartWrite(std::byte address)
                {
                    return {address << 1};
                }

                inline Start StartWrite(uint8_t address)
                {
                    return StartWrite(std::byte{address});
                }

                inline Start StartRead(std::byte address)
                {
                    return {(address << 1) | std::byte{0x01}};
                }

                inline Start StartRead(uint8_t address)
                {
                    return StartRead(std::byte{address});
                }

                template <std::size_t LEN>
                struct Write : public std::array<std::byte, LEN>
                {
                };

                struct WriteFrom
                {
                    const std::span<const std::byte> source;
                };

                template <std::size_t LEN>
                struct Read : public std::array<std::byte, LEN>
                {
                };

                struct ReadInto
                {
                    std::span<std::byte> target;
                };

                struct Stop
                {
                };

                struct _WriteCommand
                {
                    const std::span<const std::byte> data;
                };

                struct _ReadCommand
                {
                    std::span<std::byte> data;
                };

                template <typename... Args>
                struct CollectionBuilder
                {
                    CollectionBuilder(Args... args)
                        : container(std::make_tuple(std::forward<Args>(args)...))
                    {
                    }

                    std::tuple<Args...> container;
                };

                struct Collection
                {
                    template <typename... Args>
                    Collection(CollectionBuilder<Args...> &builder)
                        : commands()
                    {
                        commands.reserve(sizeof...(Args) + 1);
                        std::apply([this](auto &&...args)
                                   { ((this->Add(args)), ...); }, builder.container);
                        if (not std::holds_alternative<Stop>(commands.back()))
                        {
                            commands.push_back(Stop());
                        }
                    }

                    void Add(Start &cmd)
                    {
                        commands.push_back(cmd);
                    }

                    template <std::size_t LEN>
                    void Add(Write<LEN> &cmd)
                    {
                        commands.push_back(_WriteCommand{.data = std::span(cmd)});
                    }

                    void Add(WriteFrom &cmd)
                    {
                        commands.push_back(_WriteCommand({.data = cmd.source}));
                    }

                    template <std::size_t LEN>
                    void Add(Read<LEN> &cmd)
                    {
                        commands.push_back(_ReadCommand({.data = std::span(cmd)}));
                    }

                    void Add(ReadInto &cmd)
                    {
                        commands.push_back(_ReadCommand{.data = cmd.target});
                    }

                    void Add(Stop &cmd)
                    {
                        commands.push_back(cmd);
                    }

                    std::vector<std::variant<Start, _WriteCommand, _ReadCommand, Stop>> commands;
                };

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

                virtual void Execute(trx::Collection collection) = 0;
                template <typename... Args>
                void Execute(trx::CollectionBuilder<Args...> &builder)
                {
                    Execute(trx::Collection(builder));
                }
            };

            inline std::vector<uint8_t> ScanBus(
                iBusMaster &bus,
                Units::Second timeout = 50_ms,
                uint8_t start_address = 0x01,
                uint8_t end_address = 0x7F)
            {
                if (start_address > end_address)
                {
                    throw std::invalid_argument("Start address must be less than or equal to end address");
                }
                if (end_address > 0x7F)
                {
                    throw std::invalid_argument("End address must be less than or equal to 0x7F");
                }
                std::vector<uint8_t> alive_addresses;
                for (auto cnt = start_address; cnt <= end_address; cnt += 1)
                {
                    namespace t = alfalfa::hal::two_wire::trx;
                    try
                    {
                        auto y = t::CollectionBuilder(t::StartNone(cnt, true));
                        bus.Execute(y);
                        alive_addresses.push_back(cnt);
                    }
                        catch(const std::exception& e)
                    {
                        // do nothing
                    }
                }
                return alive_addresses;
            }

        }
    }
}
