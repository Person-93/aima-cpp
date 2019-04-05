#pragma once

#include <type_traits>


namespace aima::util {
    template< typename, template< typename > typename >
    struct is_instance : public std::false_type {};

    template< typename T, template< typename > typename U >
    struct is_instance<U<T>, U> : public std::true_type {};

    template< typename T, template< typename > typename U >
    constexpr bool is_instance_v = is_instance<T, U>();

    template< typename ... Args >
    using first_entity = std::tuple_element<0, Args...>;
}
