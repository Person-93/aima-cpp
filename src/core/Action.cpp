#include "Action.hpp"
#include <string>

// IWYU pragma: no_include <map>
// used by AttributesMixin

using std::string;
using std::string_view;

namespace aima::core {
    const string_view noOpName      = "NoOp";
    const string_view attributeName = "name";

    bool Action::isNoOp() const {
        return attributes.at( string( attributeName )) == noOpName;
    }

    Action::Action( string_view name ) {
        attributes[ string( attributeName ) ] = name;
    }

    std::string_view Action::getName() const {
        return attributes.at( string( attributeName ));
    }

    const Action& Action::noOp() {
        static Action action( noOpName );
        return action;
    }

    bool operator==( const Action& a, const Action& b ) {
        return a.getName() == b.getName();
    }
}
