#pragma once

#include <unordered_map>  // IWYU pragma: export
#include <ostream>        // IWYU pragma: export
#include <string_view>    // IWYU pragma: export

namespace aima::util {
    /**
     * A mixin that provides methods for setting and getting attributes
     * @tparam T The type. Uses CRTP.
     */
    template< class T >
    class AttributesMixin {
    protected:
        AttributesMixin() = default;

        /**
         * Attribute names and values should be literal strings or strings with static storage duration
         * @param name
         * @param value
         */
        void setAttribute( std::string_view name, std::string_view value ) {
            attributes.insert( { name, value } );
        }

        std::string_view getAttribute( std::string_view name ) const {
            return attributes.at( name );
        }

        virtual ~AttributesMixin() = default;

        friend std::ostream& operator<<( std::ostream& out, const AttributesMixin& item ) {
            out << '[';
            bool first = true;
            for ( const auto&[key, value]: item.attributes ) {
                if ( first ) { first = false; }
                else out << ',';
                out << key << "=" << value;
            }
            return out << ']';
        }

    private:
        std::unordered_map<std::string_view, std::string_view> attributes;
    };
}
