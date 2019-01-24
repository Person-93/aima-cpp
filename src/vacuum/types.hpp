#pragma once

#include <iosfwd>

namespace aima::vacuum {
    enum class LocationState { CLEAN, DIRTY };
    struct Location { unsigned int x; unsigned int y; };

    std::ostream& operator<<( std::ostream& out, LocationState state );

    std::ostream& operator<<( std::ostream& out, const Location& location );

    bool operator==( Location a, Location b ) noexcept;
}
