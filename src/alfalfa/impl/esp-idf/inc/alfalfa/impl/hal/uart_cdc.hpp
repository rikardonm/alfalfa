#pragma once

#include <cstdint>
#include <span>

#include <alfalfa/hal/uart.hpp>
#include <alfalfa/hal/pin.hpp>

#include <driver/uart.h>


namespace alfalfa
{
namespace hal
{
namespace uart
{

    class CDCModule : public iModule
    {
    public:
        CDCModule(
            const uint32_t tx_queue_size = 512,
            const uint32_t rx_queue_size = 512);
        virtual ~CDCModule() override;
        void SetBaudrate(const uint32_t baud_rate) override;
    };

    class CDCStreamSink : public alfalfa::lib::IStreamSink<std::byte>
    {
    public:
        CDCStreamSink() = default;
        virtual ~CDCStreamSink() override = default;
        virtual void WriteFrom(const std::byte& data) override;
        virtual void WriteFrom(const std::span<const std::byte> data) override;
    };

    class CDCStreamSource : public alfalfa::lib::IStreamSource<std::byte>
    {
    public:
        CDCStreamSource() = default;
        virtual ~CDCStreamSource() override = default;
        virtual std::optional<std::byte> ReadOne() override;
        virtual bool ReadInto(std::span<std::byte> target) override;
    };

}
}
}
