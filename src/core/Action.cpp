#include "Action.hpp"
#include <iostream>
#include "util/define_logger.hpp"

using std::string_view;
using aima::core::Action;

DEFINE_LOGGER( Action );

namespace {
    const string_view noOpName = "NoOp"; // NOLINT(cert-err58-cpp)
}

bool Action::isNoOp() const {
    return name == noOpName;
}

Action::Action( string_view name ) noexcept : name( name ) { TRACE; }

std::unique_ptr<Action> Action::noOp() noexcept {
    return std::make_unique<Action>( noOpName );
}

void Action::print( std::ostream& out ) const {
    out << "name=" << name;
}

std::ostream& aima::core::operator<<( std::ostream& out, const Action& action ) {
    out << '[';
    action.print( out );
    out << ']';
    return out;
}

std::unique_ptr<Action> aima::core::Action::clone() const {
    return std::unique_ptr<Action>( new Action( *this ));
}
