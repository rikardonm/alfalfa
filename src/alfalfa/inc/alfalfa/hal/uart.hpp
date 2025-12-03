#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <memory>

#include <alfalfa/lib/types/streams.hpp>


namespace alfalfa
{
namespace hal
{
namespace uart
{

    /**
     * @brief An interface for a simple, symmetrically-buffered Module communication module.
     *
     */
    class iModule
    {
    public:
        virtual ~iModule() = default;

        virtual void SetBaudrate(const uint32_t baud_rate) = 0;

        std::shared_ptr<alfalfa::lib::IStreamSource<std::byte>> Rx()
        {
            return _rx;
        }

        std::shared_ptr<alfalfa::lib::IStreamSink<std::byte>> Tx()
        {
            return _tx;
        }

    protected:
        std::shared_ptr<alfalfa::lib::IStreamSource<std::byte>> _rx;
        std::shared_ptr<alfalfa::lib::IStreamSink<std::byte>> _tx;
    };

}
}
}
