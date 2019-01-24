#include <ostream>

#include <core/Environment.hpp>
#include <core/Percept.hpp>
#include <core/Action.hpp>

#include "SimpleViewer.hpp"

using std::endl;
using namespace aima::viewer;

SimpleViewer::SimpleViewer( std::ostream& out ) : out( out ) {}

void SimpleViewer::notify( std::string_view message ) {
    out << "Message: " << message << endl;
}

void SimpleViewer::agentAdded( const Agent& agent, const Environment& source ) {
    out << "Agent " << agent.id() << " added" << endl;
}

void SimpleViewer::agentActed( const Agent& agent,
                               const Percept& percept,
                               const Action& action,
                               const Environment& environment ) {
    out << "Agent " << agent.id() << " acted.\n"
        << "Percept: " << percept << "\n"
        << "Action: " << action << "\n"
        << endl;
}
