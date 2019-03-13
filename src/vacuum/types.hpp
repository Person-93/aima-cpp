#pragma once

#include <iosfwd>
#include <util/PrintableMixin.hpp>

namespace aima::vacuum {
    enum class LocationState { CLEAN, DIRTY };

    std::ostream& operator<<( std::ostream& out, LocationState state );

    struct Location : util::PrintableMixin<Location> {
        Location() : x( 0 ), y( 0 ) {}

        Location( const Location& ) = default;

        Location( Location&& ) = default;

        Location( unsigned long x, unsigned long y ) : x( x ), y( y ) {}

        Location& operator=( const Location& ) = default;

        Location& operator=( Location&& ) = default;

        unsigned long x;
        unsigned long y;

        void print( std::ostream& out ) const {
            out << '(' << x << ',' << y << ')';
        }
    };

    bool operator==( Location a, Location b ) noexcept;
}
