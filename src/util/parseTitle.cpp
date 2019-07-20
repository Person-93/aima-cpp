#include "parseTitle.hpp"
#include <boost/core/demangle.hpp>
#include <unordered_map>

std::string_view aima::util::detail::parseTitle( std::type_index typeIndex ) {
    static std::unordered_map<std::type_index, std::string> titles;

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
