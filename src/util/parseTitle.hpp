#pragma once

#include <typeindex>
#include <string_view>

namespace aima::util {
    namespace detail {
        std::string_view parseTitle( std::type_index typeIndex );
    }

    template< typename T >
    std::string_view parseTitle() {
        return detail::parseTitle( typeid( T ));
    }

    template< typename T >
    std::string_view parseTitle( const T& t ) {
        return detail::parseTitle( typeid( t ));
    }
}
