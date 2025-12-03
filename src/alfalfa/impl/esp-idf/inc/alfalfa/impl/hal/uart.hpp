#pragma once

#include <cstdint>
#include <span>

#include <alfalfa/hal/uart.hpp>
#include <alfalfa/hal/pin.hpp>

#include <driver/uart.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>


namespace alfalfa
{
namespace hal
{
namespace uart
{

    class Module : public iModule
    {
    public:
        Module(
            const uart_port_t module_num,
            const ::alfalfa::hal::Pin tx_pin,
            const ::alfalfa::hal::Pin rx_pin,
            const uint32_t tx_queue_size = 1024,
            const uint32_t rx_queue_size = 1024);

        virtual ~Module() override;

        void SetBaudrate(const uint32_t baudrate) override;

    protected:
        const uart_port_t _uart_num;
        QueueHandle_t _queue = nullptr;
    };

    class UartStreamSink : public alfalfa::lib::IStreamSink<std::byte>
    {
    public:
        UartStreamSink(uart_port_t uart_num) : _uart_num(uart_num) {}
        virtual ~UartStreamSink() override;
        virtual void WriteFrom(const std::byte& data) override;
        virtual void WriteFrom(const std::span<const std::byte> data) override;
    private:
        const uart_port_t _uart_num;
    };

    class UartStreamSource : public alfalfa::lib::IStreamSource<std::byte>
    {
    public:
        UartStreamSource(uart_port_t uart_num) : _uart_num(uart_num) {}
        virtual ~UartStreamSource() override;
        virtual std::optional<std::byte> ReadOne() override;
        virtual bool ReadInto(std::span<std::byte> target) override;
    private:
        const uart_port_t _uart_num;
    };

}
}
}
