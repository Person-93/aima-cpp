#pragma once

#include <boost/core/demangle.hpp>
#include <unordered_map>
#include <typeindex>

namespace aima::util {
    template< typename T >
    std::string_view parseTitle() {
        static std::unordered_map<std::type_index, std::string> titles;

        auto typeIndex = std::type_index( typeid( T ));
        auto& title = titles[ typeIndex ];

        if ( title.empty()) {
            auto raw  = boost::core::demangle( typeIndex.name());
            auto name = raw.substr( raw.find_last_of( "::" ) + 1 );

            // add spaces before each capital letter except the first character
            for ( size_t i = 1; i < name.size(); ++i )
                if ( isupper( name[ i ] )) {
                    name.insert( i, 1, ' ' );
                    ++i;
                }

            title = name;
        }

        return title;
    }
}
