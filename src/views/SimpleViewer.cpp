#include "SimpleViewer.hpp"
#include <ostream>          // IWYU pragma: keep
#include "core/Percept.hpp"
#include "core/Action.hpp"
#include "core/Agent.hpp"
#include "util/define_logger.hpp"


using std::endl;
using namespace aima::viewer;
using namespace aima::core;

DEFINE_LOGGER( SimpleViewer );

SimpleViewer::SimpleViewer( std::ostream& out ) : out( &out ) { TRACE; }

void SimpleViewer::notify( std::string_view message ) {
    TRACE;
    *out << "Message: " << message << endl;
}

void SimpleViewer::agentAdded( const Agent& agent, const Environment& ) {
    TRACE;
    *out << "Agent " << agent.id() << " added\n" << endl;
}

void SimpleViewer::agentActed( const Agent& agent,
                               const Percept& percept,
                               const Action& action,
                               const Environment& ) {
    TRACE;
    *out << "Agent " << agent.id() << " acted.\n"
         << "Percept: " << percept << "\n"
         << "Action: " << action << "\n"
         << endl;
}
