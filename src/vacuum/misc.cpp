#include "misc.hpp"
#include <ostream>

namespace aima::vacuum {
    std::ostream& operator<<( std::ostream& out, LocationState state ) {
        switch ( state ) {
            case LocationState::CLEAN: return out << "CLEAN";
            case LocationState::DIRTY: return out << "DIRTY";
        }
    }

    std::ostream& operator<<( std::ostream& out, const Location& location ) {
        return out << '(' << location.x << ',' << location.y << ')';
    }
}
