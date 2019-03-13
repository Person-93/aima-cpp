#include <sstream>
#include <stdexcept>

#include "StatefulVacuumAgent.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"
#include "util/parseTitle.hpp"
#include "BasicVacuumEnvironment.hpp"

using aima::vacuum::StatefulVacuumAgent;
using namespace aima::core;
using std::out_of_range;

StatefulVacuumAgent::StatefulVacuumAgent( unsigned long x, unsigned long y ) : state( x, y ) {
    for ( size_t i = 0; i < x; ++i )
        for ( size_t j = 0; j < y; ++j )
            state( i, j ) = false;
}

const Action& StatefulVacuumAgent::execute( const Percept& percept ) {
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
    for ( size_t i = 0, s1 = state.size1(); i < s1; ++i )
        for ( size_t j = 0, s2 = state.size2(); j < s2; ++j )
            if ( !state( i, j )) return false;
    return true;
}
