//
// Created by Aleksey Dorofeev on 12/02/2020.
//

#pragma once

#include <utility>
#include <vector>
#include <type_traits>

// specialize a type for all of needed containers
namespace is_stl_container_impl {
    template<typename T>
    struct is_stl_container : std::false_type {
    };
    template <typename T, std::size_t N>
    struct is_stl_container<std::array <T,N>> : std::true_type {
    };
    template<typename... Args>
    struct is_stl_container<std::vector<Args...>> : std::true_type {
    };
    template<typename ...Args>
    struct is_stl_container<std::initializer_list<Args...>> : std::true_type {
    };
    //tbd...
}

// type trait to utilize the implementation type traits as well as decay the type
template <typename T>
struct is_stl_container {
    static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};

void is_stl_container_test() {
    std::cout << "is_stl_container " << is_stl_container<int>::value << std::endl;
    std::cout << "is_stl_container " << is_stl_container<int[5]>::value << std::endl;
    std::cout << "is_stl_container " << is_stl_container<std::vector<int>>::value << std::endl;
    std::cout << "is_stl_container " << is_stl_container<std::initializer_list<int>>::value << std::endl;
}
