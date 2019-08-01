#pragma once

#include <iosfwd>
#include <functional>

namespace aima::vacuum {
    enum class LocationState { CLEAN, DIRTY };

    std::ostream& operator<<( std::ostream& out, LocationState state );

    struct Location {
        unsigned long x;
        unsigned long y;
    };

    std::ostream& operator<<( std::ostream& out, const Location& location );

    inline bool operator==( const Location& a, const Location& b ) noexcept {
        return a.x == b.x && a.y == b.y;
    }

    inline bool operator!=( const Location& a, const Location& b ) noexcept {
        return !( a == b );
    }
}

namespace std {
    template<>
    struct less<aima::vacuum::Location> {
        constexpr bool operator()( const aima::vacuum::Location& a, const aima::vacuum::Location& b ) const noexcept {
            return a.x < b.x || a.y < b.y;
        }
    };
}
