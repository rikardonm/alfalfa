#pragma once

#include <span>
#include <cstddef>
#include <type_traits>
#include <concepts>


namespace alfalfa
{
namespace lib
{

    template<typename T>
    std::span<std::byte> AsBytes(T& object)
    // requires std::same_as<T, typename std::remove_const<T>::type>
    {
        return std::as_writable_bytes(
            std::span<std::byte>{reinterpret_cast<std::byte*>(std::addressof(object)), sizeof(object)}
        );
    }

    // // see https://stackoverflow.com/a/7792799/3474172
    // template<typename T>
    // std::span<std::byte> AsBytes(const T& object)
    // requires (not std::same_as<T, typename std::remove_const<T>::type>)
    // {
    //     return std::as_bytes(
    //         std::span<std::byte>{reinterpret_cast<std::byte*>(std::addressof(object)), sizeof(object)}
    //     );
    // }

}
}

