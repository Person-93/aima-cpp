#pragma once

#include <vector>
#include <iosfwd>

namespace aima::path_finder {
    struct Point { float x; float y; };

    inline bool operator==( const Point& a, const Point& b ) {
        return a.x == b.x && a.y == b.y;
    }

    inline bool operator!=( const Point& a, const Point& b ) {
        return !( a == b );
    }

    std::ostream& operator<<( std::ostream& out, Point point );
}
