#include <alfalfa/impl/hal/uart.hpp>

#include <cstdint>
#include <span>


namespace alfalfa
{
namespace hal
{
namespace uart
{

    Module::Module(
        const uart_port_t module_num,
        const ::alfalfa::hal::Pin tx_pin,
        const ::alfalfa::hal::Pin rx_pin,
        const uint32_t tx_queue_size,
        const uint32_t rx_queue_size)
        : _uart_num(module_num)
    {
        if (_uart_num >= UART_NUM_MAX)
        {
            throw std::invalid_argument("Invalid UART module number");
        }

        uart_driver_install(_uart_num, rx_queue_size, tx_queue_size, 10, &_queue, 0);
        uart_set_pin(_uart_num, tx_pin.IoIndex(), rx_pin.IoIndex(), UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

        _rx = std::make_shared<alfalfa::hal::uart::UartStreamSource>(_uart_num);
        _tx = std::make_shared<alfalfa::hal::uart::UartStreamSink>(_uart_num);
    }

    Module::~Module()
    {
        uart_driver_delete(_uart_num);
    }

    void Module::SetBaudrate(uint32_t baudrate)
    {
        uart_set_baudrate(_uart_num, baudrate);
    };

    UartStreamSink::~UartStreamSink() = default;

    void UartStreamSink::WriteFrom(const std::byte& data)
    {
        uart_write_bytes(_uart_num, reinterpret_cast<const char*>(&data), 1);
    };

    void UartStreamSink::WriteFrom(const std::span<const std::byte> data)
    {
        if (data.empty())
        {
            return;
        }

        const auto len = uart_write_bytes(_uart_num, reinterpret_cast<const char*>(data.data()), data.size());
        if (len < 0)
        {
            throw std::runtime_error("Failed to write data to UART");
        }
    };

    UartStreamSource::~UartStreamSource() = default;

    std::optional<std::byte> UartStreamSource::ReadOne()
    {
        std::byte val;
        auto ret = uart_read_bytes(_uart_num, &val, 1, 0);
        if (ret == 1)
        {
            return {val};
        }
        else
        {
            return std::nullopt;
        }
    };

    bool UartStreamSource::ReadInto(std::span<std::byte> target)
    {
        auto ret =uart_read_bytes(_uart_num, target.data(), target.size_bytes(), 0);
        return ret != 0;
    };

}
}
}
