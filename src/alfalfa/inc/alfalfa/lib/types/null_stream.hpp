#pragma once

#include <cstdint>
#include <array>
#include <tuple>
#include <iostream>
#include <memory>
#include <tuple>
#include <span>

namespace alfalfa
{
namespace lib
{

    /**
     * @brief A type that can be used to create a null stream.
     *
     * This class is used to create a null stream that does not perform any operations.
     * See: https://stackoverflow.com/a/11826666/3474172
     */
    class NullStreamBuf : public std::streambuf
    {
    public:
        int overflow(int_type c) { return c; }
    };

    /**
     * @brief A stream type that doesn't output nothing to nowhere.
     *
     */
    class NullStream : public std::ostream
    {
    public:
        NullStream() : std::ostream(&_buf) {}
    private:
        NullStreamBuf _buf;
    };

}
}
