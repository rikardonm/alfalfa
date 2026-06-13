#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <memory>
#include <streambuf>
#include <ranges>


namespace alfalfa
{
namespace lib
{

    /**
     * @brief This class represents a sink for a stream of T data elements.
     *
     * @tparam T
     */
    template<typename T>
    class IStreamSink
    {
    public:
        virtual ~IStreamSink() = default;

        virtual void WriteFrom(const T& data) = 0;
        virtual void WriteFrom(const std::span<const T> data) = 0;

        template<typename TT>
        requires std::ranges::contiguous_range<TT>
        void WriteFrom(TT& data)
        {
            const auto bytes = std::as_bytes(std::span(data));
            WriteFrom(bytes);
        }
    };

    /**
     * @brief This class represents a source for a stream of T data elements.
     *
     * Retrieval operations remove data from the stream; this operation cannot be reverted.
     *
     * @tparam T
     */
    template<typename T>
    class IStreamSource
    {
    public:
        virtual ~IStreamSource() = default;

        virtual std::optional<T> ReadOne() = 0;
        virtual bool ReadInto(std::span<T> target) = 0;
    };


    /**
     * @brief StreamSinkSourceAdapter class adapts a stream buffer to a sink/source interface.
     *
     */
    class StreamSinkSourceAdapter : public std::streambuf
    {
    private:
        std::shared_ptr<alfalfa::lib::IStreamSink<std::byte>> _sink;
        std::shared_ptr<alfalfa::lib::IStreamSource<std::byte>> _source;
    public:
        StreamSinkSourceAdapter(std::shared_ptr<alfalfa::lib::IStreamSink<std::byte>> sink,
                                std::shared_ptr<alfalfa::lib::IStreamSource<std::byte>> source)
            : std::streambuf()
            , _sink(sink)
            , _source(source)
        {
        }

        virtual ~StreamSinkSourceAdapter()
        {
            sync();
        }

        int sync() override
        {
            // Flush the UART buffer
            return 0;
        }

        int_type overflow(int_type c = traits_type::eof()) override
        {
            if (_sink and c != traits_type::eof())
            {
                _sink->WriteFrom(std::byte{static_cast<char_type>(c)});
            }
            return c;
        }

        int_type underflow() override
        {
            if (_source)
            {
                const auto byte = _source->ReadOne();
                if (byte)
                {
                    return std::to_integer<int_type>(byte.value());
                }
            }
            return traits_type::eof();
        }
    };

}
}
