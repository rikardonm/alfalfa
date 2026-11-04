#pragma once

#include <cstdint>
#include <span>
#include <cstddef>
#include <type_traits>
#include <concepts>
#include <vector>


namespace alfalfa
{
namespace lib
{

    std::span<std::byte> AsBytes(std::span<uint8_t> object)
    {
        return std::span<std::byte>{reinterpret_cast<std::byte*>(std::addressof(object.front())), object.size()};
    }

    template<typename T>
    std::span<std::byte> AsBytes(T& object)
    requires std::same_as<T, typename std::remove_const<T>::type>
    {
        return std::span<std::byte>{reinterpret_cast<std::byte*>(std::addressof(object)), sizeof(object)};
    }

    std::span<const std::byte> AsBytes(std::span<const uint8_t> object)
    {
        return std::span<const std::byte>{reinterpret_cast<const std::byte*>(std::addressof(object.front())), object.size()};
    }

    // see https://stackoverflow.com/a/7792799/3474172
    template<typename T>
    std::span<const std::byte> AsBytes(T& object)
    requires (not std::same_as<T, typename std::remove_const<T>::type>)
    {
        return std::span<const std::byte>{reinterpret_cast<const std::byte*>(std::addressof(object)), sizeof(object)};
    }

}
}
