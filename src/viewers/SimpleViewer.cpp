#include <ostream>

#include "core/Percept.hpp"  // IWYU pragma: keep
#include "core/Action.hpp" // IWYU pragma: keep
#include "core/Agent.hpp"

#include "SimpleViewer.hpp" // IWYU pragma: associated

// IWYU pragma: no_include <map>
// IWYU pragma: no_include "util/AttributesMixin.hpp"
// it wants to include them to print the percept, but percept is self-contained

using std::endl;
using namespace aima::viewer;
using namespace aima::core;

SimpleViewer::SimpleViewer( std::ostream& out ) : out( out ) {}

void SimpleViewer::notify( std::string_view message ) {
    out << "Message: " << message << endl;
}

void SimpleViewer::agentAdded( const Agent& agent, const Environment& ) {
    out << "Agent " << agent.id() << " added\n" << endl;
}

void SimpleViewer::agentActed( const Agent& agent,
                               const Percept& percept,
                               const Action& action,
                               const Environment& ) {
    out << "Agent " << agent.id() << " acted.\n"
        << "Percept: " << percept << "\n"
        << "Action: " << action << "\n"
        << endl;
}
