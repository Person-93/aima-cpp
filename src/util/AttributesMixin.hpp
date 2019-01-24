#pragma once

#include <map>
#include <ostream>

namespace aima::util {
    /**
     * A mixin that provides methods for setting and getting attributes
     * @tparam T The type. Uses CRTP.
     */
    template< class T >
    class AttributesMixin {
        AttributesMixin() = default;

        friend T;
    public:
        void setAttribute( std::string_view name, std::string_view value ) {
            attributes[ std::string( name ) ] = value;
        }

        std::string_view getAttribute( std::string_view name ) const {
            return attributes.at( std::string( name ));
        }

        virtual ~AttributesMixin() = default;

        friend std::ostream& operator<<( std::ostream& out, const T& item ) {
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
        std::map<std::string, std::string> attributes;
    };
}
