#include <ostream>

#include "types.hpp"

namespace aima::vacuum {
    std::ostream& operator<<( std::ostream& out, LocationState state ) {
        switch ( state ) {
            case LocationState::CLEAN:
                return out << "CLEAN";
            case LocationState::DIRTY:
                return out << "DIRTY";
            default:
                return out << "INVALID STATE";
        }
    }

    bool operator==( Location a, Location b ) noexcept {
        return a.x == b.x && a.y == b.y;
    }
}
