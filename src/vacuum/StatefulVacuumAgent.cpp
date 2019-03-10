#include <sstream>
#include <stdexcept>

#include "StatefulVacuumAgent.hpp"
#include "core/Percept.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"
#include "util/parseTitle.hpp"
#include "BasicVacuumEnvironment.hpp"

using aima::vacuum::StatefulVacuumAgent;
using aima::core::Action;
using std::out_of_range;

StatefulVacuumAgent::StatefulVacuumAgent( unsigned x, unsigned y ) : x( x ), y( y ), state( x, y ) {}

const Action& StatefulVacuumAgent::execute( const aima::core::Percept& percept ) {
    auto p = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
    if ( !p ) throw out_of_range( "Agent received percept of unrecognized type" );

    auto location = p->agentLocation;
    state( location.x, location.y ) = true;

    if ( p->agentLocationState == LocationState::DIRTY ) return BasicVacuumEnvironment::ACTION_SUCK;
    if ( allClean()) return Action::noOp();
    if ( location == Location{ 0, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_RIGHT;
    if ( location == Location{ 1, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_LEFT;
    else {
        std::ostringstream s;
        s << util::parseTitle<StatefulVacuumAgent>() << " received unrecognized percept: " << percept;
        throw out_of_range( s.str());
    }
}

bool StatefulVacuumAgent::allClean() {
    for ( unsigned i = 0; i < x; ++i )
        for ( unsigned j = 0; j < y; ++j )
            if ( !state( i, j )) return false;
    return true;
}
