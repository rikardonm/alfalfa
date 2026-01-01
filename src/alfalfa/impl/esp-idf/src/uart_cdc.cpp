#include <alfalfa/impl/hal/uart_cdc.hpp>

#include <cstdint>
#include <span>

#include <driver/uart.h>
#include <driver/usb_serial_jtag.h>


namespace alfalfa
{
namespace hal
{
namespace uart
{

    CDCModule::CDCModule(
        const uint32_t tx_queue_size,
        const uint32_t rx_queue_size)
    {
        auto cfg = usb_serial_jtag_driver_config_t{
            .tx_buffer_size = tx_queue_size,
            .rx_buffer_size = rx_queue_size};

        auto ret = usb_serial_jtag_driver_install(&cfg);
        if (ret != ESP_OK)
        {
            throw std::runtime_error("Failed to install USB CDC driver");
        }
        _rx = std::make_shared<alfalfa::hal::uart::CDCStreamSource>();
        _tx = std::make_shared<alfalfa::hal::uart::CDCStreamSink>();
    }

    CDCModule::~CDCModule()
    {
        usb_serial_jtag_driver_uninstall();
    }

    void CDCModule::SetBaudrate(uint32_t baudrate)
    {
        // Do nothing, baud is automatically managed over USB
    };

    void CDCStreamSink::WriteFrom(const std::byte& data)
    {
        const auto len = usb_serial_jtag_write_bytes(reinterpret_cast<const char*>(&data), 1, 0);
        if (len < 0)
        {
            throw std::runtime_error("Failed to write data to UART");
        }
    };

    void CDCStreamSink::WriteFrom(const std::span<const std::byte> data)
    {
        if (data.empty())
        {
            return;
        }

        const auto len = usb_serial_jtag_write_bytes(reinterpret_cast<const char*>(data.data()), data.size(), 0);
        if (len < 0)
        {
            throw std::runtime_error("Failed to write data to UART");
        }
    };

    std::optional<std::byte> CDCStreamSource::ReadOne()
    {
        std::byte val;
        auto ret = usb_serial_jtag_read_bytes(&val, 1, 0);
        if (ret == 1)
        {
            return {val};
        }
        else
        {
            return std::nullopt;
        }
    };

    bool CDCStreamSource::ReadInto(std::span<std::byte> target)
    {
        auto ret = usb_serial_jtag_read_bytes(target.data(), target.size_bytes(), 0);
        return ret != 0;
    };

}
}
}
