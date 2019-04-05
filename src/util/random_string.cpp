#include "random_string.hpp"
#include <random>

std::string aima::util::random_string( std::string::size_type length ) {
    static auto& characters = "0123456789"
                              "abcdefghijklmnopqrstuvwxyz"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937                                          rg{ std::random_device{}() };
    thread_local static std::uniform_int_distribution<std::string::size_type> pick( 0, sizeof( characters ) - 2 );

    std::string s;
    s.reserve( length );

    for ( std::string::size_type i = 0; i < length; ++i )
        s += characters[ pick( rg ) ];

    return s;
}
